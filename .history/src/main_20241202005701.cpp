#include "rtc_base/thread.h"
#include "rtc_base/logging.h"
#include "rtc_base/physical_socket_server.h"
#include "rtc_base/strings/json.h"
#include "modules/websocket/include/websocket.h"

#include <iostream>
#include <memory>
#include <string>
#include <thread>

// 定义信令服务器地址
#define SIGNALING_SERVER_ADDRESS "ws://192.168.0.165:80/signaling"

// 信令客户端类
class SignalingClient : public rtc::WebSocket::Observer
{
public:
    SignalingClient() : websocket_(rtc::WebSocket::Create()) {}

    // 连接到信令服务器
    bool Connect(const std::string& server_address)
    {
        // 设置 WebSocket 观察者
        websocket_->RegisterObserver(this);

        // 启动网络线程
        network_thread_ = rtc::Thread::Create();
        if (!network_thread_->Start())
        {
            std::cerr << "Failed to start network thread." << std::endl;
            return false;
        }

        // 连接到 WebSocket
        if (!websocket_->Connect(server_address, network_thread_.get()))
        {
            std::cerr << "Failed to connect to signaling server at " << server_address << std::endl;
            return false;
        }

        std::cout << "Connected to signaling server at " << server_address << std::endl;
        return true;
    }

    // 发送消息
    void SendMessage(const std::string& message)
    {
        websocket_->Send(message);
        std::cout << "Sent: " << message << std::endl;
    }

    // WebSocket 消息回调
    void OnMessage(const std::string& message) override
    {
        std::cout << "Received message: " << message << std::endl;
    }

    // WebSocket 关闭回调
    void OnClose(int code, const std::string& reason) override
    {
        std::cerr << "WebSocket closed. Code: " << code << ", Reason: " << reason << std::endl;
    }

    // WebSocket 错误回调
    void OnError(const std::string& error) override
    {
        std::cerr << "WebSocket error: " << error << std::endl;
    }

private:
    rtc::scoped_refptr<rtc::WebSocket> websocket_;
    std::unique_ptr<rtc::Thread> network_thread_;
};

int main()
{
    // 初始化日志
    rtc::LogMessage::LogToDebug(rtc::LS_INFO);
    rtc::LogMessage::SetMinLogSeverity(rtc::LS_INFO);

    std::cout << "Starting signaling client..." << std::endl;

    // 创建信令客户端实例
    SignalingClient signaling_client;
    if (!signaling_client.Connect(SIGNALING_SERVER_ADDRESS))
    {
        std::cerr << "Failed to connect to signaling server." << std::endl;
        return -1;
    }

    // 发送一个示例消息到信令服务器
    signaling_client.SendMessage("{\"type\": \"listStreamers\"}");

    // 等待接收信令消息
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Shutting down signaling client." << std::endl;

    return 0;
}
