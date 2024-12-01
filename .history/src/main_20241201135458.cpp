#include "rtc_base/ref_count.h"
#include "rtc_base/logging.h"
#include "rtc_base/log_sinks.h"

int main() {
    // 设置日志级别
    rtc::LogMessage::LogToDebug(rtc::LS_INFO);
    RTC_LOG(LS_INFO) << "Testing WebRTC initialization";
    return 0;
}