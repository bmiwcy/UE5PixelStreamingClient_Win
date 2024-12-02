#pragma once

// Include core WebRTC headers
#include <api/rtc_error.h> // Handles WebRTC-specific error types.
#include <api/data_channel_interface.h> // Interface for WebRTC DataChannel.
#include <api/media_stream_interface.h> // Interface for WebRTC media streams.
#include <api/peer_connection_interface.h> // PeerConnection core interface.
#include <api/video/video_frame_buffer.h> // Interface for video frame buffer handling.
#include <api/video/i420_buffer.h> // Utility for I420 video frames.
#include <api/video_codecs/video_encoder_factory.h> // Video encoder factory interface.
#include <modules/audio_device/include/audio_device_defines.h> // Defines for audio devices.
#include <rtc_base/copy_on_write_buffer.h> // Efficient buffer utility.
