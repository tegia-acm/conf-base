using namespace base::sockets;


///
///     Stops data listening on the socket and closes all opened client sockets.
///
std::string TCPServerSocket::stop(std::shared_ptr<message_t> const& message, nlohmann::json const& route_params) noexcept
{
    //
    //  Check the caller type (internal message or web request):
    //
    auto& output_data = message->data.contains("responce") && message->data["responce"].contains("data") ? message->data["responce"]["data"] : message->data;

    if (auto [err_code, err_str] = close_socket(); err_code == SUCCESS)
    {
        output_data[RETURN_CODE] = OK_STATUS;
        output_data[RETURN_INFO] = err_str.has_value() ? std::move(*err_str) : "Server TCP/IP socket was successfully closed.";
        NOUT(*output_data[RETURN_INFO].get_ptr<const nlohmann::json::string_t*>())
    }
    else
    {
        output_data[RETURN_CODE] = ERROR_STATUS;
        output_data["error"]["code"] = err_code;
        output_data["error"][RETURN_INFO] = err_str.has_value() ? std::move(*err_str) : "Unspecified error happened.";
        EOUT(*output_data["error"][RETURN_INFO].get_ptr<const nlohmann::json::string_t*>())
    }

   return *output_data[RETURN_CODE].get_ptr<const nlohmann::json::string_t*>();
}


///
/// Stops socket listening, close file descriptor (if required) and clear all the data
//
error_data TCPServerSocket::close_socket() noexcept
{
    if (is_closed()) 
    {
        //stop_listener();
        return std::make_pair(SUCCESS, std::make_optional(std::string{"Server TCP/IP socket is already closed."}));
    }

    stop_listener();

    return std::make_pair(SUCCESS, std::nullopt);
}


///
///  Closes all opened sockets.
///
error_data TCPServerSocket::close_socket_descriptors() noexcept
{
    try
    {
        std::scoped_lock lock{ socket_guard_ };

        //
        //  Close client sockets first (if any exists)
        //
	    for (auto const& [client_socket_id, client_address] : client_sockets_)
	    {
		    ::close(client_socket_id);
        }
	    client_sockets_.clear();

        //
        // Close server socket (if it is initialized) and clear the data
        //
        if (!is_closed()) 
        {
            if (is_closed_.store(::close(server_socket_id_) == SUCCESS); !is_closed())
            {
                return std::make_pair(errno, std::make_optional(fmt::format("Server TCP/IP socket on '{}:{}' was not closed: {}.", 
                    ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port), ::strerror(errno))));
            }

            server_socket_id_ = INVALID_SOCKET;
            ::memset(&server_address_, 0, sizeof(server_address_));
        }
	}
    catch(const std::exception& e)
    {
       return std::make_pair(INTERNAL_SERVER_ERROR, std::make_optional(fmt::format("Error happened while closing TCP/IP socket on '{}:{}': {}.", 
            ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port), e.what())));
    }

    return std::make_pair(SUCCESS, std::nullopt);
}


///
/// Sets flag to stop socket data listening and waits until thread terminates.
///
void TCPServerSocket::stop_listener() noexcept
{
    is_connected_.store(false);

    if (listener_)
    {
       if (listener_->joinable()) listener_->join();
        listener_.reset(nullptr);
    }
}