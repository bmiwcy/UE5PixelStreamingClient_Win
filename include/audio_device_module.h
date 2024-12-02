// audio_device_module.h
#pragma once

#include "modules/audio_device/include/audio_device.h"
#include "rtc_base/ref_counted_object.h"
#include <memory>

class SimpleAudioCapturer : public webrtc::AudioDeviceModule {
private:
    // 状态变量
    bool initialized_ = false;
    bool recording_initialized_ = false;
    static constexpr int kSampleRate = 48000;
    static constexpr int kNumChannels = 2;

    // WebRTC的音频回调
    webrtc::AudioTransport* audio_callback_ = nullptr;

public:
    SimpleAudioCapturer() = default;
    ~SimpleAudioCapturer() override = default;

    // 主要初始化和终止
    int32_t Init() override {
        initialized_ = true;
        return 0;
    }
    int32_t Terminate() override {
        initialized_ = false;
        return 0;
    }
    bool Initialized() const override { return initialized_; }

    // 音频传输
    int32_t RegisterAudioCallback(webrtc::AudioTransport* audio_callback) override {
        audio_callback_ = audio_callback;
        return 0;
    }

    // 设备枚举
    int16_t PlayoutDevices() override { return 0; }
    int16_t RecordingDevices() override { return 0; }
    int32_t PlayoutDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize], 
                             char guid[webrtc::kAdmMaxGuidSize]) override { return 0; }
    int32_t RecordingDeviceName(uint16_t index, char name[webrtc::kAdmMaxDeviceNameSize],
                               char guid[webrtc::kAdmMaxGuidSize]) override { return 0; }

    // 设备选择
    int32_t SetPlayoutDevice(uint16_t index) override { return 0; }
    int32_t SetPlayoutDevice(WindowsDeviceType device) override { return 0; }
    int32_t SetRecordingDevice(uint16_t index) override { return 0; }
    int32_t SetRecordingDevice(WindowsDeviceType device) override { return 0; }

    // 音频传输初始化和控制
    int32_t PlayoutIsAvailable(bool* available) override { 
        *available = true;
        return 0;
    }
    int32_t InitPlayout() override { return 0; }
    bool PlayoutIsInitialized() const override { return false; }
    int32_t StartPlayout() override { return 0; }
    int32_t StopPlayout() override { return 0; }
    bool Playing() const override { return false; }

    int32_t RecordingIsAvailable(bool* available) override {
        *available = true;
        return 0;
    }
    int32_t InitRecording() override {
        recording_initialized_ = true;
        return 0;
    }
    bool RecordingIsInitialized() const override { return recording_initialized_; }
    int32_t StartRecording() override { return 0; }
    int32_t StopRecording() override { return 0; }
    bool Recording() const override { return false; }

    // 其他必要的方法
    int32_t ActiveAudioLayer(AudioLayer* audioLayer) const override { return 0; }
    int32_t PlayoutDelay(uint16_t* delayMS) const override {
        *delayMS = 0;
        return 0;
    }
    
    // 所有不支持的功能返回 -1
    int32_t InitSpeaker() override { return -1; }
    bool SpeakerIsInitialized() const override { return false; }
    int32_t InitMicrophone() override { return -1; }
    bool MicrophoneIsInitialized() const override { return false; }

    // 声音控制相关
    int32_t SpeakerVolumeIsAvailable(bool* available) override { return -1; }
    int32_t SetSpeakerVolume(uint32_t volume) override { return -1; }
    int32_t SpeakerVolume(uint32_t* volume) const override { return -1; }
    int32_t MaxSpeakerVolume(uint32_t* maxVolume) const override { return -1; }
    int32_t MinSpeakerVolume(uint32_t* minVolume) const override { return -1; }
    int32_t MicrophoneVolumeIsAvailable(bool* available) override { return -1; }
    int32_t SetMicrophoneVolume(uint32_t volume) override { return -1; }
    int32_t MicrophoneVolume(uint32_t* volume) const override { return -1; }
    int32_t MaxMicrophoneVolume(uint32_t* maxVolume) const override { return -1; }
    int32_t MinMicrophoneVolume(uint32_t* minVolume) const override { return -1; }

    // 静音控制
    int32_t SpeakerMuteIsAvailable(bool* available) override { return -1; }
    int32_t SetSpeakerMute(bool enable) override { return -1; }
    int32_t SpeakerMute(bool* enabled) const override { return -1; }
    int32_t MicrophoneMuteIsAvailable(bool* available) override { return -1; }
    int32_t SetMicrophoneMute(bool enable) override { return -1; }
    int32_t MicrophoneMute(bool* enabled) const override { return -1; }

    // 立体声支持
    int32_t StereoPlayoutIsAvailable(bool* available) const override { return -1; }
    int32_t SetStereoPlayout(bool enable) override { return -1; }
    int32_t StereoPlayout(bool* enabled) const override { return -1; }
    int32_t StereoRecordingIsAvailable(bool* available) const override { return -1; }
    int32_t SetStereoRecording(bool enable) override { return -1; }
    int32_t StereoRecording(bool* enabled) const override { return -1; }

    // 内置效果
    bool BuiltInAECIsAvailable() const override { return false; }
    bool BuiltInAGCIsAvailable() const override { return false; }
    bool BuiltInNSIsAvailable() const override { return false; }
    int32_t EnableBuiltInAEC(bool enable) override { return -1; }
    int32_t EnableBuiltInAGC(bool enable) override { return -1; }
    int32_t EnableBuiltInNS(bool enable) override { return -1; }
};