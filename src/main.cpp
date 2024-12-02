#include "signaling_client.h"
#include "webrtc_manager.h"
#include <iostream>

#define SIGNALING_SERVER_ADDRESS "ws://192.168.0.165:80/signaling"

int main() {
    SignalingClient signalingClient;
    WebRTCManager webrtcManager;

    try {
        std::cout << "Attempting to initialize WebRTCManager..." << std::endl;
        if (!webrtcManager.Initialize()) {
            std::cerr << "Failed to initialize WebRTC." << std::endl;
            return -1;
        }
        std::cout << "WebRTCManager initialized successfully." << std::endl;

        // 创建 PeerConnection
        webrtcManager.CreatePeerConnection();
        
        // 设置 WebRTC 回调
        webrtcManager.SetAnswerCallback([&signalingClient](const std::string& sdp) {
            std::cout << "Generated answer, sending to signaling server" << std::endl;
            signalingClient.SendAnswer(sdp);
        });

        webrtcManager.SetIceCandidateCallback(
            [&signalingClient](const std::string& candidate, 
                              const std::string& sdpMid, 
                              int sdpMLineIndex) {
                std::cout << "Got ICE candidate, sending to signaling server" << std::endl;
                signalingClient.SendIceCandidate(candidate, sdpMid, sdpMLineIndex);
            });

    } catch (const std::exception& e) {
        std::cerr << "Exception caught during initialization: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "Program is proceeding normally..." << std::endl;

    if (!signalingClient.Connect(SIGNALING_SERVER_ADDRESS)) {
        std::cerr << "Failed to connect to signaling server." << std::endl;
        return -1;
    }

    std::cout << "Successfully connected to signaling server." << std::endl;

    // 设置信令服务器回调
    signalingClient.SetMessageCallback([](const std::string& message) {
        std::cout << "Message received in callback: " << message << std::endl;
    });

    signalingClient.SetOfferCallback([&webrtcManager](const std::string& sdp) {
        std::cout << "Received offer, handling in WebRTC..." << std::endl;
        webrtcManager.HandleOffer(sdp);
    });

    signalingClient.SetIceCandidateCallback(
        [&webrtcManager](const std::string& candidate, 
                        const std::string& sdpMid, 
                        int sdpMLineIndex) {
            std::cout << "Received ICE candidate, handling in WebRTC..." << std::endl;
            webrtcManager.HandleIceCandidate(candidate, sdpMid, sdpMLineIndex);
        });

    signalingClient.SendSignalingMessage("{\"type\": \"listStreamers\"}");
    std::cout << "Sent 'listStreamers' request to signaling server." << std::endl;

    // 改为无限循环来保持连接，监听信令消息并处理
    while (true) {
        signalingClient.PollEvents(1000);
    }

    signalingClient.Disconnect();
    std::cout << "Disconnected from signaling server." << std::endl;

    return 0;
}
