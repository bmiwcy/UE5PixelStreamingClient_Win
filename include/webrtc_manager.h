#ifndef WEBRTC_MANAGER_H
#define WEBRTC_MANAGER_H

#include <api/peer_connection_interface.h>
#include <api/create_peerconnection_factory.h>
#include <rtc_base/thread.h>
#include <functional>
#include <memory>
#include <iostream>

#include "video_stream_configuration.h"

// Forward declaration of SetSessionDescriptionObserver
class SetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
public:
    static rtc::scoped_refptr<SetSessionDescriptionObserver> Create() {
        return rtc::scoped_refptr<SetSessionDescriptionObserver>(
            new rtc::RefCountedObject<SetSessionDescriptionObserver>());
    }
    
    void OnSuccess() override {
        std::cout << "Set session description success" << std::endl;
    }
    void OnFailure(webrtc::RTCError error) override {
        std::cerr << "Set session description failed: " << error.message() << std::endl;
    }

protected:
    SetSessionDescriptionObserver() = default;
};

class WebRTCManager : public webrtc::PeerConnectionObserver,
                     public webrtc::CreateSessionDescriptionObserver {
public:
    WebRTCManager();
    ~WebRTCManager();

    bool Initialize();
    void CreatePeerConnection();
    void HandleOffer(const std::string& sdp);
    void HandleIceCandidate(const std::string& candidate, 
                           const std::string& sdpMid, 
                           int sdpMLineIndex);

    // Set callback functions
    void SetAnswerCallback(std::function<void(const std::string&)> callback) {
        answer_callback_ = std::move(callback);
    }
    void SetIceCandidateCallback(
        std::function<void(const std::string&, const std::string&, int)> callback) {
        ice_candidate_callback_ = std::move(callback);
    }

    // PeerConnectionObserver implementation
    void OnSignalingChange(
        webrtc::PeerConnectionInterface::SignalingState new_state) override;
    void OnDataChannel(
        rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
    void OnIceGatheringChange(
        webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
    void OnConnectionChange(
        webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
    void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override {
        // Handle received media track
    }
    void OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override {
        // Handle removed track
    }

    // CreateSessionDescriptionObserver implementation
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;
    
    // AddRef() and Release() from reference counting
    void AddRef() const override { ++ref_count_; }
    rtc::RefCountReleaseStatus Release() const override {
        if (--ref_count_ == 0) {
            return rtc::RefCountReleaseStatus::kDroppedLastRef;
        }
        return rtc::RefCountReleaseStatus::kOtherRefsRemained;
    }

private:
    // Threads
    std::unique_ptr<rtc::Thread> network_thread_;
    std::unique_ptr<rtc::Thread> worker_thread_;
    std::unique_ptr<rtc::Thread> signaling_thread_;

    // WebRTC factory and connection
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;

    // Callback functions
    std::function<void(const std::string&)> answer_callback_;
    std::function<void(const std::string&, const std::string&, int)> ice_candidate_callback_;

    mutable int ref_count_ = 0;
};

#endif // WEBRTC_MANAGER_H
