// Add these defines before any includes
#define WEBRTC_WIN 1
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

// WebRTC includes
#include "api/ref_counted_base.h"  // Add this first
#include "rtc_base/thread.h"
#include "rtc_base/ssl_adapter.h"
#include "api/create_peerconnection_factory.h"

// Standard includes
#include <iostream>
#include <memory>

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