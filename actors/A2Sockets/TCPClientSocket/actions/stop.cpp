using namespace base::sockets;


///
///  Stops data listening and disconnects from the server socket.
///
std::string TCPClientSocket::disconnect(std::shared_ptr<message_t> const& message, nlohmann::json const& route_params) noexcept
{
    //
    //  Check the caller type (internal message or web request):
    //
    auto& output_data = message->data.contains("responce") && message->data["responce"].contains("data") ? message->data["responce"]["data"] : message->data;

    //
    // NOTE: here we're using synchronous call!!!
    //
    if (auto [err_code, err_str] = close_socket(); err_code == SUCCESS)
    {
        output_data[RETURN_CODE] = OK_STATUS;
        output_data[RETURN_INFO] = err_str.has_value() ? std::move(*err_str) : "Client TCP/IP socket was successfully closed.";
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
error_data TCPClientSocket::close_socket() noexcept
{
    is_connected_.store(false);
   
    if (is_closed()) 
    {
        stop_listener();
        return std::make_pair(SUCCESS, std::make_optional(std::string{"Client TCP/IP socket is already closed."}));
    }

    if (auto rc = close_socket_descriptor(); rc.first != SUCCESS)   return std::move(rc);

    stop_listener();

    return std::make_pair(SUCCESS, std::nullopt);
 }


///
/// Closes file descriptor (if required) and clear all the data
//
error_data TCPClientSocket::close_socket_descriptor() noexcept
{
    std::scoped_lock lock { socket_guard_ };

    if (!is_closed()) 
    {
        if (is_closed_.store(::close(client_socket_id_) == SUCCESS); !is_closed())
        {
            return std::make_pair(errno, std::make_optional(fmt::format("Client TCP/IP socket was not closed: {}.", strerror(errno))));
        }

        client_socket_id_ = INVALID_SOCKET;
        ::memset(&server_address_, 0, sizeof(server_address_));
    }

    return std::make_pair(SUCCESS, std::nullopt);
 }


///
/// Sets flag to stop socket data listening and waits until thread terminates.
///
void TCPClientSocket::stop_listener() noexcept
{
    is_connected_.store(false);

    if (listener_)
    {
        if (listener_->joinable()) listener_->join();
        listener_.reset(nullptr);
    }
}