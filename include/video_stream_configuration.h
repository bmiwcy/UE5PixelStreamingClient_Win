#ifndef STREAM_CONFIG_H
#define STREAM_CONFIG_H

#include <unordered_set>
#include <string_view>
#include <optional>

namespace stream_config
{
    enum class VideoStreamCodec
    {
        VP8,
        VP9,
        H264
    };

    std::optional<VideoStreamCodec> stringToVideoStreamCodec(std::string_view value);

    class VideoStreamConfiguration
    {
        std::unordered_set<VideoStreamCodec> m_forcedCodecs;
        bool m_forceHardwareAcceleration;
        bool m_useSoftwareEncoderDecoder;

        VideoStreamConfiguration(
            std::unordered_set<VideoStreamCodec> forcedCodecs,
            bool forceHardwareAcceleration,
            bool useSoftwareEncoderDecoder);

    public:
        VideoStreamConfiguration(const VideoStreamConfiguration& other) = default;
        VideoStreamConfiguration(VideoStreamConfiguration&& other) = default;
        virtual ~VideoStreamConfiguration() = default;

        static VideoStreamConfiguration create();
        static VideoStreamConfiguration create(std::unordered_set<VideoStreamCodec> forcedCodecs);
        static VideoStreamConfiguration create(
            std::unordered_set<VideoStreamCodec> forcedCodecs,
            bool forceHardwareAcceleration,
            bool useSoftwareEncoderDecoder);

        [[nodiscard]] const std::unordered_set<VideoStreamCodec>& forcedCodecs() const;
        [[nodiscard]] bool forceHardwareAcceleration() const;
        [[nodiscard]] bool useSoftwareEncoderDecoder() const;

        VideoStreamConfiguration& operator=(const VideoStreamConfiguration& other) = default;
        VideoStreamConfiguration& operator=(VideoStreamConfiguration&& other) = default;
    };

} // namespace stream_config

#endif // STREAM_CONFIG_H
