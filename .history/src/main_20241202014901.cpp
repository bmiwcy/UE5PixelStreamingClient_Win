#include <libwebsockets.h>
#include <iostream>
#include <cstring>

// Define signaling server address
#define SIGNALING_SERVER_ADDRESS "ws://192.168.0.165:80/signaling"

// WebSocket callback function
static int websocket_callback(struct lws* wsi, enum lws_callback_reasons reason,
                              void* user, void* in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        std::cout << "WebSocket connection established." << std::endl;
        lws_callback_on_writable(wsi); // Request writeable event
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
        std::cout << "Received: " << std::string((const char*)in, len) << std::endl;
        break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
    {
        const char* message = "{\"type\": \"listStreamers\"}";
        unsigned char buffer[LWS_PRE + 512];
        std::memset(buffer, 0, sizeof(buffer));
        std::memcpy(&buffer[LWS_PRE], message, std::strlen(message));
        lws_write(wsi, &buffer[LWS_PRE], std::strlen(message), LWS_WRITE_TEXT);
        std::cout << "Sent: " << message << std::endl;
        break;
    }

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

int main()
{
    struct lws_context_creation_info context_info;
    struct lws_client_connect_info connect_info;
    struct lws_context* context;

    // Initialize context info
    std::memset(&context_info, 0, sizeof(context_info));
    context_info.port = CONTEXT_PORT_NO_LISTEN;
    context_info.protocols = nullptr;
    context_info.gid = -1;
    context_info.uid = -1;

    // Define protocol list
    static const struct lws_protocols protocols[] = {
        { "example-protocol", websocket_callback, 0, 512 },
        { nullptr, nullptr, 0, 0 }
    };
    context_info.protocols = protocols;

    // Create the WebSocket context
    context = lws_create_context(&context_info);
    if (!context)
    {
        std::cerr << "Failed to create WebSocket context." << std::endl;
        return -1;
    }

    // Initialize connect info
    std::memset(&connect_info, 0, sizeof(connect_info));
    connect_info.context = context;
    connect_info.address = "192.168.0.165";
    connect_info.port = 80;
    connect_info.path = "/signaling";
    connect_info.host = lws_canonical_hostname(context);
    connect_info.origin = "origin";
    connect_info.protocol = protocols[0].name;

    // Connect to the signaling server
    if (!lws_client_connect_via_info(&connect_info))
    {
        std::cerr << "Failed to connect to WebSocket server." << std::endl;
        lws_context_destroy(context);
        return -1;
    }

    std::cout << "Connecting to signaling server at " << SIGNALING_SERVER_ADDRESS << std::endl;

    // Main loop for handling WebSocket events
    for (int i = 0; i < 10; ++i)
    {
        lws_service(context, 1000); // Poll for events with a 1-second timeout
    }

    // Destroy the WebSocket context
    lws_context_destroy(context);
    return 0;
}
