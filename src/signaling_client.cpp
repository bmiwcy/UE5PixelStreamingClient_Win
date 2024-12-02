#include "signaling_client.h"
#include <libwebsockets.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <json/json.h>

// WebSocket implementation details for SignalingClient
struct SignalingClient::Impl {
    struct lws_context* context = nullptr;
    struct lws* wsi = nullptr;
    std::function<void(const std::string&)> message_callback;
    std::vector<std::string> message_queue; // Queue for outgoing messages
    std::string receive_buffer;  // Buffer for incoming messages
	bool is_receiving_sdp = false; // Flag to indicate we're receiving SDP

    // WebSocket callback for handling events
    static int WebSocketCallback(struct lws* wsi, enum lws_callback_reasons reason,
                                 void* user, void* in, size_t len) {
        auto* impl = reinterpret_cast<Impl*>(lws_wsi_user(wsi));
        switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            std::cout << "WebSocket connection established." << std::endl;
            lws_callback_on_writable(wsi); // Request writable callback
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE: 
		 	 	if (impl && impl->message_callback) {
 	 	        std::string receivedMessage(static_cast<const char*>(in), len);
 	 	        std::cout << "Received message: " << receivedMessage << std::endl;

 	 	        // 检查是否是新的 JSON 消息开始
 	 	        if (receivedMessage.find("{\"type\":\"offer\"") != std::string::npos) {
 	 	            std::cout << "===== Found start of offer message =====" << std::endl;
 	 	            impl->receive_buffer.clear();
 	 	            impl->is_receiving_sdp = true;
 	 	            impl->receive_buffer = receivedMessage;  // 不要忘记加入第一部分
 	 	        }
 	 	        // 累积消息
 	 	        else if (impl->is_receiving_sdp) {
 	 	            impl->receive_buffer += receivedMessage;
	 	 
 	 	            // 检查是否是最后一个片段 (寻找消息结束的 }")
 	 	            if (receivedMessage.find("\"}") != std::string::npos) {
 	 	                std::cout << "===== Received complete offer message =====" << std::endl;
 	 	                // 处理完整的消息
 	 	                impl->message_callback(impl->receive_buffer);
 	 	                impl->is_receiving_sdp = false;
 	 	                impl->receive_buffer.clear();
 	 	            }
 	 	        } else {
 	 	            // 非 offer 消息直接处理
 	 	            impl->message_callback(receivedMessage);
 	 	        }
 	 	    }
 	 	    break;
    		

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            if (impl && !impl->message_queue.empty()) {
                const std::string& message = impl->message_queue.front();
                unsigned char buffer[LWS_PRE + 512];
                memset(buffer, 0, sizeof(buffer));
                memcpy(&buffer[LWS_PRE], message.c_str(), message.length());

                int ret = lws_write(wsi, &buffer[LWS_PRE], message.length(), LWS_WRITE_TEXT);
                if (ret < 0) {
                    std::cerr << "Failed to send message: " << message << std::endl;
                } else {
                    std::cout << "Message sent successfully: " << message << std::endl;
                    impl->message_queue.erase(impl->message_queue.begin()); // Remove sent message
                }

                // Request writable callback if more messages are queued
                if (!impl->message_queue.empty()) {
                    lws_callback_on_writable(wsi);
                }
            }
            break;

        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            std::cerr << "WebSocket connection error!" << std::endl;
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            std::cout << "WebSocket connection closed." << std::endl;
            break;

        default:
            break;
        }
        return 0;
    }
};

// Constructor: Initialize the implementation
SignalingClient::SignalingClient() : pImpl(new Impl) {}

// Destructor: Clean up resources
SignalingClient::~SignalingClient() {
    Disconnect();
    delete pImpl;
}

// Connect to the signaling server
bool SignalingClient::Connect(const std::string& server_url) {
    struct lws_context_creation_info context_info = {};
    struct lws_client_connect_info connect_info = {};

    static const struct lws_protocols protocols[] = {
        { "example-protocol", Impl::WebSocketCallback, 0, 512 },
        { nullptr, nullptr, 0, 0 }
    };
    context_info.protocols = protocols;

    pImpl->context = lws_create_context(&context_info);
    if (!pImpl->context) {
        std::cerr << "Failed to create WebSocket context." << std::endl;
        return false;
    }

    connect_info.context = pImpl->context;
    connect_info.address = "192.168.0.165";
    connect_info.port = 80;
    connect_info.path = "/signaling";
    connect_info.host = lws_canonical_hostname(pImpl->context);
    connect_info.origin = "origin";
    connect_info.protocol = protocols[0].name;
    connect_info.userdata = pImpl;

    pImpl->wsi = lws_client_connect_via_info(&connect_info);
    if (!pImpl->wsi) {
        std::cerr << "Failed to connect to WebSocket server." << std::endl;
        lws_context_destroy(pImpl->context);
        pImpl->context = nullptr;
        return false;
    }

    return true;
}

// Send a message through the WebSocket connection
void SignalingClient::SendSignalingMessage(const std::string& message) {
    if (pImpl->wsi) {
        pImpl->message_queue.push_back(message);
        lws_callback_on_writable(pImpl->wsi); // Request writable callback
        std::cout << "Queued message: " << message << std::endl;
    } else {
        std::cerr << "WebSocket connection is not available for sending messages." << std::endl;
    }
}

// Set the callback for receiving messages
void SignalingClient::SetMessageCallback(std::function<void(const std::string&)> callback) {
    pImpl->message_callback = [this, callback](const std::string& message) {
        std::cout << "Raw message received: " << message << std::endl;
        if (callback) {
            callback(message);
        }
        ParseMessage(message); // Parse received message
    };
}

// Poll for WebSocket events and process them
void SignalingClient::PollEvents(int timeout_ms) {
    if (pImpl->context) {
        lws_service(pImpl->context, timeout_ms);
    }
}

// Disconnect from the signaling server
void SignalingClient::Disconnect() {
    if (pImpl->context) {
        lws_context_destroy(pImpl->context);
        pImpl->context = nullptr;
        pImpl->wsi = nullptr;
    }
}

// Parse a received message (e.g., listStreamers response)
void SignalingClient::ParseMessage(const std::string& message) {
    std::cout << "Parsing message: " << message << std::endl;

    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errs;

    std::istringstream stream(message);
    if (Json::parseFromStream(reader, stream, &root, &errs)) {
        const std::string type = root["type"].asString();
        std::cout << "Message type: " << type << std::endl;

        if (type == "streamerList") {
            streamer_list_.clear();
            for (const auto& id : root["ids"]) {
                streamer_list_.push_back(id.asString());
            }
            std::cout << "Streamer list received: ";
            for (const auto& streamer : streamer_list_) {
                std::cout << streamer << " ";
            }
            std::cout << std::endl;

            if (!streamer_list_.empty()) {
                std::string subscribeMessage = "{\"type\": \"subscribe\", \"streamerId\": \"" + streamer_list_[0] + "\"}";
                std::cout << "Generated subscribe message: " << subscribeMessage << std::endl;
                SendSignalingMessage(subscribeMessage);
            }
        }
		else if (type == "offer") {
		    const std::string sdp = root["sdp"].asString();
    		std::cout << "===== SignalingClient: Processing offer in ParseMessage =====" << std::endl;

    		if (offer_callback_) {
    		    std::cout << "===== SignalingClient: Calling offer_callback_ =====" << std::endl;
    		    offer_callback_(sdp);
    		} else {
    		    std::cout << "===== SignalingClient: No offer_callback_ set! =====" << std::endl;
    		}
		}
		else if (type == "iceCandidate") {
		    if (ice_candidate_callback_) {
		        const std::string candidate = root["candidate"].asString();
		        const std::string sdpMid = root["sdpMid"].asString();
		        int sdpMLineIndex = root["sdpMLineIndex"].asInt();
		        ice_candidate_callback_(candidate, sdpMid, sdpMLineIndex);
		    }
		}
    } else {
        std::cerr << "Failed to parse message: " << errs << std::endl;
    }
}

const std::vector<std::string>& SignalingClient::GetStreamerList() const {
    return streamer_list_;
}

void SignalingClient::SetOfferCallback(std::function<void(const std::string& sdp)> callback) {
    offer_callback_ = std::move(callback);
}

void SignalingClient::SetIceCandidateCallback(
    std::function<void(const std::string& candidate, 
                      const std::string& sdpMid, 
                      int sdpMLineIndex)> callback) {
    ice_candidate_callback_ = std::move(callback);
}

void SignalingClient::SendAnswer(const std::string& sdp) {
    Json::Value message;
    message["type"] = "answer";
    message["sdp"] = sdp;
    
    Json::FastWriter writer;
    std::string jsonString = writer.write(message);
    SendSignalingMessage(jsonString);
}

void SignalingClient::SendIceCandidate(const std::string& candidate, 
                                     const std::string& sdpMid, 
                                     int sdpMLineIndex) {
    Json::Value message;
    message["type"] = "iceCandidate";
    message["candidate"] = candidate;
    message["sdpMid"] = sdpMid;
    message["sdpMLineIndex"] = sdpMLineIndex;
    
    Json::FastWriter writer;
    std::string jsonString = writer.write(message);
    SendSignalingMessage(jsonString);
}
