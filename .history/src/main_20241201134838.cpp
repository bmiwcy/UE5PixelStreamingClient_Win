#include <iostream>  
#include "api/create_peerconnection_factory.h"
#include "rtc_base/thread.h"
#include "rtc_base/ssl_adapter.h"

int main() {
    // Initialize SSL
    rtc::InitializeSSL();

    // Create threads for WebRTC
    std::unique_ptr<rtc::Thread> network_thread = rtc::Thread::CreateWithSocketServer();
    network_thread->Start();

    std::cout << "WebRTC test program initialized." << std::endl;

    // Cleanup
    rtc::CleanupSSL();
    return 0;
}