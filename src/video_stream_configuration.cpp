#include "video_stream_configuration.h"

namespace stream_config
{
    std::optional<VideoStreamCodec> stringToVideoStreamCodec(std::string_view value)
    {
        if (value == "VP8")
        {
            return VideoStreamCodec::VP8;
        }
        else if (value == "VP9")
        {
            return VideoStreamCodec::VP9;
        }
        else if (value == "H264")
        {
            return VideoStreamCodec::H264;
        }
        return std::nullopt;
    }
}
