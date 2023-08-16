using namespace base::sockets;

///
///     Sends message to the TCP/IP client socket connected to the server.
///     The required IP address and port number should be specified.
///
std::string TCPServerSocket::send(std::shared_ptr<message_t> const& message, nlohmann::json const& route_params) noexcept
{
    //
    //  Check the caller type (internal message or web request):
    //
    bool const use_route_params = !route_params.is_null();
    auto const& input_data = use_route_params ? route_params : message->data;
    auto& output_data = use_route_params ? message->data["responce"]["data"] : message->data;

    const auto process_error = [&output_data] (const int error_code, std::string&& error_str) -> std::string
    {
        LERROR(error_str)
        EOUT(error_str)
        output_data[RETURN_CODE] = ERROR_STATUS;
        output_data["error"]["code"] = error_code;        
        output_data["error"][RETURN_INFO] = std::move(error_str);
        return ERROR_STATUS;
    };

    //
    //  Get client socket's IP address and port number from the input data:
    //
    auto const socket_address = get_socket_address(input_data);
    if (!socket_address.first.has_value())
    {
        return process_error(BAD_REQUEST_ERROR, std::string("Cannot send TCP/IP message: no IP address is specified in the input data."));
    }
    in_addr ip_address{};
    if (::inet_aton((*socket_address.first).c_str(), &ip_address) == 0)
    {
        return process_error(BAD_FORMAT_ERROR, std::string("Cannot send TCP/IP message: invalid IP address is specified in the input data."));
    }
    
    if (!socket_address.second.has_value())
    {
        return process_error(BAD_REQUEST_ERROR, std::string("Cannot send TCP/IP message: no port number is specified in the input data or it's invalid."));
    }
    in_port_t const port = ::htons(*socket_address.second);

    //
    //  Get message to send from the input data:
    //
    const nlohmann::json::string_t* ptr = (input_data.contains(TCPIP_MESSAGE) && input_data[TCPIP_MESSAGE].is_string()) 
        ? input_data[TCPIP_MESSAGE].get_ptr<const nlohmann::json::string_t*>() : nullptr;
    if (ptr == nullptr || ptr->empty())
    {
        return process_error(BAD_REQUEST_ERROR, fmt::format("Cannot send TCP/IP message to '{}:{}': no message body was set in input data.", 
            *socket_address.first, *socket_address.second));
    }

    //
    //  Send message to the server:
    //
    auto [error_code, error_string] = send_message(ip_address, port, std::string_view(ptr->data(), ptr->size()));
    if (error_code != SUCCESS)
    {
       return process_error(BAD_REQUEST_ERROR, error_string.has_value() ? std::move(*error_string) : std::string("Unspecified error happened."));
    }

    output_data[RETURN_INFO] = error_string.has_value() ? std::move(*error_string) : 
        fmt::format("TCP/IP message was sent to the client on '{}:{}'.", *socket_address.first, *socket_address.second);
    NOUT(*output_data[RETURN_INFO].get_ptr<const nlohmann::json::string_t*>())
    output_data[RETURN_CODE] = OK_STATUS;
    return OK_STATUS;
}


///
///     Sends message to the specified TCP/IP client socket connected to the server.
///
error_data TCPServerSocket::send_message(in_addr ip_address, in_port_t port_number, std::string_view message_body) noexcept
{
    //
    // Check if socket is already initialized and running:
    //
    if (!is_connected())
    {
        return std::make_pair(BAD_REQUEST_ERROR, std::make_optional(std::string("Cannot send TCP/IP message: server socket is not running.")));
    }

    // 
    // Calculate the size of TCP/IP message to send:
    //
    constexpr std::size_t sig_len = std::strlen(TEGIA_SIGNATURE);
    constexpr std::size_t siz_len = sizeof(std::uint64_t);
    std::uint64_t const  msg_len = static_cast<std::uint64_t>(message_body.size());
    std::size_t const new_buffer_size = sig_len + siz_len + msg_len;

    try
    {
        //
        // Note! We need exclusive access to the buffer.
        //
        std::scoped_lock lock { socket_guard_ };

        //
        //  Find if specified client socket exists (is connected to the server)
        //
        auto const it = std::find_if(client_sockets_.cbegin(), client_sockets_.cend(), 
            [port_number, ip_address](auto const& item){ return item.second.sin_port == port_number && item.second.sin_addr.s_addr == ip_address.s_addr; });
        if (it == client_sockets_.end())
        {
            return std::make_pair(NOT_FOUND_ERROR, std::make_optional(fmt::format("Cannot send TCP/IP message: no client socket found for '{}:{}'.", 
                ::inet_ntoa(ip_address), ::htons(port_number))));
        }
        auto const socket_id = it->first;

        // 
        // Allocate the required buffer or use existing if i's enough:
        //
        if (new_buffer_size > send_buffer_size)
        {
            if (new_buffer_size <= MAXIMAL_BUFFER_SIZE)
            {  
                if (void* const new_buffer = ::realloc(send_buffer, new_buffer_size); new_buffer != nullptr)
                {
                    send_buffer = new_buffer;
                    send_buffer_size = new_buffer_size;
                } 
            }

            if (new_buffer_size > send_buffer_size)
            {
                return std::make_pair(ENOMEM, std::make_optional(std::string("Cannot send TCP/IP message: not enough memory is available.")));
            }
        }

        // 
        // Create the correct Tegia TCP/IP message - with the header containing signature and body size:
        //
        ::memcpy(send_buffer, reinterpret_cast<const void*>(TEGIA_SIGNATURE), sig_len);
        ::memcpy(reinterpret_cast<void*>(reinterpret_cast<char*>(send_buffer) + sig_len), reinterpret_cast<const void*>(&msg_len), siz_len);
        ::memcpy(reinterpret_cast<void*>(reinterpret_cast<char*>(send_buffer) + sig_len + siz_len), reinterpret_cast<const void*>(message_body.data()), msg_len);

        //
        //  Send all message data
        //
        ssize_t total_len = 0L;
        while (static_cast<std::size_t>(total_len) < new_buffer_size)
        {
            ssize_t const bytes_sent = 
                ::send(socket_id, reinterpret_cast<void*>(reinterpret_cast<char*>(send_buffer) + total_len), new_buffer_size - total_len, 0);
            if (bytes_sent == -1)
            {
                return std::make_pair(errno, std::make_optional(fmt::format("Error happened while sending TCP/IP message to client on '{}:{}': {}.", 
                    ::inet_ntoa(ip_address), ::htons(port_number), ::strerror(errno))));
            }
            total_len += bytes_sent;
        }

    }
    catch (const std::exception& e)
    {
        return std::make_pair(INTERNAL_SERVER_ERROR, std::make_optional(fmt::format("Error happened while sending TCP/IP message to client on '{}:{}': {}.", 
            ::inet_ntoa(ip_address), ::htons(port_number), e.what())));
    }
    
    return std::make_pair(SUCCESS, std::make_optional(fmt::format("TCP/IP message was sent to the client on '{}:{}'.", ::inet_ntoa(ip_address), ::htons(port_number))));
}