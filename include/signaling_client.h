#ifndef SIGNALING_CLIENT_H
#define SIGNALING_CLIENT_H

#include <string>
#include <vector>
#include <functional>
#include <json/json.h>

class SignalingClient {
public:
    SignalingClient();
    ~SignalingClient();

    bool Connect(const std::string& server_url);
    void SendSignalingMessage(const std::string& message);
    void SetMessageCallback(std::function<void(const std::string&)> callback);
    void PollEvents(int timeout_ms);
    void Disconnect();

	void SetOfferCallback(std::function<void(const std::string& sdp)> callback);
	void SetIceCandidateCallback(std::function<void(const std::string& candidate, const std::string& sdpMid, int sdpMLineIndex)> callback);

	void SendAnswer(const std::string& sdp);
	void SendIceCandidate(const std::string& candidate, const std::string& sdpMid, int sdpMLineIndex);


    const std::vector<std::string>& GetStreamerList() const;

private:
    struct Impl;
    Impl* pImpl;
    std::vector<std::string> streamer_list_;

    void ParseMessage(const std::string& message);

	std::function<void(const std::string& sdp)> offer_callback_;
    std::function<void(const std::string& candidate, const std::string& sdpMid, int sdpMLineIndex)> ice_candidate_callback_;
};

#endif // SIGNALING_CLIENT_H
