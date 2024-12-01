#include "rtc_base/thread.h"
#include "rtc_base/logging.h"
#include "rtc_base/physical_socket_server.h"
#include "rtc_base/strings/json.h"
#include "modules/websocket/include/websocket.h"

#include <iostream>
#include <memory>
#include <string>

// 定义信令服务器地址
#define SIGNALING_SERVER_ADDRESS "ws://192.168.0.165:80/signaling"

class SignalingClient : public rtc::WebSocket::Observer
{
public:
    SignalingClient() : websocket_(rtc::WebSocket::Create()) {}

    bool Connect(const std::string& server_address)
    {
        // 设置观察者
        websocket_->RegisterObserver(this);

        // 连接 WebSocket
        rtc::SocketAddress server_socket_address;
        if (!server_socket_address.FromString(server_address))
        {
            std::cerr << "Invalid signaling server address: " << server_address << std::endl;
            return false;
        }

        rtc::Thread* network_thread = rtc::Thread::Create().release();
        network_thread->Start();

        rtc::AsyncSocket* async_socket = rtc::AsyncTCPSocket::Create(
            rtc::Thread::Current()->socketserver(), server_socket_address, "signaling_client");
        if (!async_socket)
        {
            std::cerr << "Failed to create AsyncTCPSocket!" << std::endl;
            return false;
        }

        return websocket_->Connect(server_address, async_socket);
    }

    void SendMessage(const std::string& message)
    {
        websocket_->Send(message);
    }

    void OnMessage(const std::string& message) override
    {
        std::cout << "Received message: " << message << std::endl;
    }

    void OnClose(int code, const std::string& reason) override
    {
        std::cerr << "WebSocket closed. Code: " << code << ", Reason: " << reason << std::endl;
    }

    void OnError(const std::string& error) override
    {
        std::cerr << "WebSocket error: " << error << std::endl;
    }

private:
    rtc::scoped_refptr<rtc::WebSocket> websocket_;
};

int main()
{
    rtc::LogMessage::LogToDebug(rtc::LS_INFO);
    rtc::LogMessage::SetMinLogSeverity(rtc::LS_INFO);

    std::cout << "Starting signaling client..." << std::endl;

    SignalingClient signaling_client;
    if (!signaling_client.Connect(SIGNALING_SERVER_ADDRESS))
    {
        std::cerr << "Failed to connect to signaling server." << std::endl;
        return -1;
    }

    signaling_client.SendMessage("{\"type\": \"listStreamers\"}");
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}
