#include "webrtc_manager.h"
#include "audio_device_module.h"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>

WebRTCManager::WebRTCManager() = default;

WebRTCManager::~WebRTCManager() {
    peer_connection_ = nullptr;
    peer_connection_factory_ = nullptr;
}

bool WebRTCManager::Initialize() {
    std::cout << "Creating signaling thread..." << std::endl << std::flush;
    signaling_thread_ = rtc::Thread::CreateWithSocketServer();
    if (!signaling_thread_->Start()) {
        std::cerr << "Failed to start signaling thread" << std::endl;
        return false; // 返回 false 确保错误路径有返回值
    }
    std::cout << "Signaling thread created successfully" << std::endl << std::flush;

    // 创建 factories 对象
    std::cout << "Creating encoder/decoder factories..." << std::endl << std::flush;
    auto audio_encoder_factory = webrtc::CreateBuiltinAudioEncoderFactory();
    if (!audio_encoder_factory) {
        std::cerr << "Failed to create audio encoder factory" << std::endl;
        return false; // 返回 false 确保错误路径有返回值
    }
    std::cout << "Audio encoder factory created" << std::endl << std::flush;

    auto audio_decoder_factory = webrtc::CreateBuiltinAudioDecoderFactory();
    if (!audio_decoder_factory) {
        std::cerr << "Failed to create audio decoder factory" << std::endl;
        return false; // 返回 false 确保错误路径有返回值
    }
    std::cout << "Audio decoder factory created" << std::endl << std::flush;

    auto video_encoder_factory = webrtc::CreateBuiltinVideoEncoderFactory();
    if (!video_encoder_factory) {
        std::cerr << "Failed to create video encoder factory" << std::endl;
        return false; // 返回 false 确保错误路径有返回值
    }
    std::cout << "Video encoder factory created" << std::endl << std::flush;

    auto video_decoder_factory = webrtc::CreateBuiltinVideoDecoderFactory();
    if (!video_decoder_factory) {
        std::cerr << "Failed to create video decoder factory" << std::endl;
        return false; // 返回 false 确保错误路径有返回值
    }
    std::cout << "Video decoder factory created" << std::endl << std::flush;

    rtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module =
        rtc::scoped_refptr<webrtc::AudioDeviceModule>(
            new rtc::RefCountedObject<SimpleAudioCapturer>()
        );

    // 创建 PeerConnectionFactory
    std::cout << "Creating PeerConnectionFactory..." << std::endl << std::flush;
    peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
        nullptr,                    // network_thread
        nullptr,                    // worker_thread
        signaling_thread_.get(),    // signaling_thread
        audio_device_module,        // default_adm
        audio_encoder_factory,      // audio_encoder_factory
        audio_decoder_factory,      // audio_decoder_factory
        std::move(video_encoder_factory),  // video_encoder_factory
        std::move(video_decoder_factory),  // video_decoder_factory
        nullptr,                    // audio_mixer
        nullptr                     // audio_processing - 暂时不使用
    );

    if (!peer_connection_factory_) {
        std::cerr << "Failed to create PeerConnectionFactory" << std::endl;
        return false; // 返回 false 确保错误路径有返回值
    }

    std::cout << "WebRTC initialization completed successfully" << std::endl << std::flush;
    return true; // 初始化成功时返回 true
}

void WebRTCManager::CreatePeerConnection() {
    webrtc::PeerConnectionInterface::RTCConfiguration config;
    // 添加 STUN/TURN 服务器
    webrtc::PeerConnectionInterface::IceServer ice_server;
    ice_server.urls.push_back("stun:stun.l.google.com:19302");
    config.servers.push_back(ice_server);

    // 创建 PeerConnection
    peer_connection_ = peer_connection_factory_->CreatePeerConnection(
        config, nullptr, nullptr, this);

    if (!peer_connection_) {
        std::cerr << "Failed to create PeerConnection" << std::endl;
        return;
    }

}

void WebRTCManager::HandleOffer(const std::string& sdp) {
    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
        webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, sdp, &error);

    if (!session_description) {
        std::cerr << "Failed to parse offer SDP: " << error.description << std::endl;
        return;
    }

    // 设置远端描述
    auto observer = SetSessionDescriptionObserver::Create();
    peer_connection_->SetRemoteDescription(observer.get(), session_description.release());

    // 创建应答
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
	options.offer_to_receive_audio = true;
    peer_connection_->CreateAnswer(this, options);
}

void WebRTCManager::HandleIceCandidate(const std::string& candidate,
                                     const std::string& sdpMid,
                                     int sdpMLineIndex) {
    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> ice_candidate(
        webrtc::CreateIceCandidate(sdpMid, sdpMLineIndex, candidate, &error));

    if (!ice_candidate) {
        std::cerr << "Failed to create ICE candidate: " << error.description << std::endl;
        return;
    }

    peer_connection_->AddIceCandidate(ice_candidate.get());
}

// PeerConnectionObserver implementation
void WebRTCManager::OnSignalingChange(
    webrtc::PeerConnectionInterface::SignalingState new_state) {
    std::cout << "Signaling state changed to: " << static_cast<int>(new_state) << std::endl;
}

void WebRTCManager::OnDataChannel(
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) {
    std::cout << "Data channel received" << std::endl;
}

void WebRTCManager::OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState new_state) {
    std::cout << "ICE gathering state changed to: " << static_cast<int>(new_state) << std::endl;
}

void WebRTCManager::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
    std::string sdp;
    if (!candidate->ToString(&sdp)) {
        std::cerr << "Failed to serialize candidate" << std::endl;
        return;
    }

    if (ice_candidate_callback_) {
        ice_candidate_callback_(sdp, candidate->sdp_mid(), candidate->sdp_mline_index());
    }
}

void WebRTCManager::OnConnectionChange(
    webrtc::PeerConnectionInterface::PeerConnectionState new_state) {
    std::cout << "Connection state changed to: " << static_cast<int>(new_state) << std::endl;
}

// CreateSessionDescriptionObserver implementation
void WebRTCManager::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
    std::cout << "Created answer, setting local description..." << std::endl;

    peer_connection_->SetLocalDescription(SetSessionDescriptionObserver::Create().get(), desc);

    //// 发送 answer SDP
    //std::string sdp;
    //if (desc->ToString(&sdp) && answer_callback_) {
    //    std::cout << "Successfully created answer SDP. Sending to signaling server..." << std::endl;
    //    answer_callback_(sdp);
    //} else {
    //    std::cerr << "Failed to convert answer description to SDP string." << std::endl;
    //}
}




void WebRTCManager::OnFailure(webrtc::RTCError error) {
    std::cerr << "Failed to create session description: " 
              << error.message() << std::endl;
}
