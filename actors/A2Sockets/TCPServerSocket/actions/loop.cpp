using namespace base::sockets;

///
///     Starts TCP/IP server socket and waits in loop client connections and data sent.
///
std::string TCPServerSocket::loop(std::shared_ptr<message_t> const& message, nlohmann::json const& route_params) noexcept
{
    //
    //  Check the caller type (internal message or web request):
    //
    bool const use_route_params = !route_params.is_null();
    auto const& input_data = use_route_params ? route_params : message->data;
    auto& output_data = use_route_params ? message->data["responce"]["data"] : message->data;

    const auto process_error = [&output_data, this] (const int error_code, std::string&& error_str) -> std::string
    {
        LERROR(error_str)
        EOUT(error_str)
        output_data[RETURN_CODE] = ERROR_STATUS;
        output_data["error"]["code"] = error_code;        
        output_data["error"]["info"] = std::move(error_str);
        return ERROR_STATUS;
    };

    //
    //  Get port number and host IP address from the input data:
    //
    auto server_socket_address = get_socket_address(input_data);
    if (!server_socket_address.second.has_value())
    {
        return process_error(BAD_REQUEST_ERROR, std::string("Cannot create TCP/IP socket: no port number is specified in the input data or it's invalid."));
    }

    //
    //  Create and initialize client TCP/IP socket:
    //
    auto [error_code, error_string] 
        = start_loop(server_socket_address.first.has_value() ? std::move(*server_socket_address.first) : std::string{}, *server_socket_address.second);
    if (error_code != SUCCESS) 
    {
       return process_error(error_code, error_string.has_value() ? std::move(*error_string) : std::string("Cannot create TCP/IP socket: unspecified error happened."));
    }

    output_data[RETURN_INFO] = error_string.has_value() ? std::move(*error_string) : std::string("TCP/IP server socket was created successfully.");
    LNOTICE(*output_data[RETURN_INFO].get_ptr<const nlohmann::json::string_t*>())
    NOUT(*output_data[RETURN_INFO].get_ptr<const nlohmann::json::string_t*>())

    output_data[RETURN_CODE] = OK_STATUS;
    return OK_STATUS;
}


//
// Creates server TCP/IP socket, binds it to the port and starts listening.
//
error_data TCPServerSocket::start_loop(std::string&& ip_address, in_port_t port_number) noexcept
{
    //
    // Convert server address data to the network format:
    //
    in_addr address{};
    address.s_addr = ::htonl(INADDR_ANY);
    ::inet_aton(ip_address.c_str(), &address);
    port_number = ::htons(port_number);

    //
    // Check if socket is already initrialized and running:
    //
    if (is_connected())
    {
        //
        //  If it is bind to the same server then it's nothing to do.
        //
        if (address.s_addr == server_address_.sin_addr.s_addr && port_number == server_address_.sin_port)
        {
            return std::make_pair(SUCCESS, std::make_optional(fmt::format("TCP/IP server socket is already bind to and listening on '{}:{}'.", 
                ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port))));
        }

        //
        //  If it is connected to another server return an error:
        //
        return std::make_pair(BAD_REQUEST_ERROR, 
            std::make_optional(fmt::format("Cannot start TCP/IP server socket with another address because it is already listening on '{}:{}'.", 
            ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port))));
    }

    //
    //  Create and initialize server TCP/IP socket and bind to the port
    //
    if (auto rc = initialize_socket(address.s_addr, port_number); rc.first != SUCCESS)
    {
        return rc;
    }

    auto const str = fmt::format("TCP/IP server socket with id = {} is listening on '{}:{}'.", 
        server_socket_id_, ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port));
    LNOTICE(str)
    NOUT(str)

    //
    // Starts listening for client connections and messages
    //
    start_listener();

    return std::make_pair(SUCCESS, std::nullopt);
}


///
/// Waits until some event(s) happen on any socket and process it
//
void TCPServerSocket::process_events() noexcept
{
    fd_set sockets_to_read{};
    //
    //  Loop waiting for the new client connection and/or data sent from the connected clients:
    //
    while (is_connected())
    {
        if (auto events_number = wait_for_events(sockets_to_read); events_number > 0)
        {
           //
            //  New client connection request has come, try to accept it:
            //
            if (FD_ISSET(server_socket_id_, &sockets_to_read))
            {
                --events_number;
                accept_client();
            }

            //
            //  Loop for the already connected clients:
            //
            auto it = client_sockets_.begin();
            while (events_number > 0 && it != client_sockets_.end())
            {
                if (FD_ISSET(it->first, &sockets_to_read))
                {
                    --events_number;
                    //
                    // Obtained data from the client. Try to read and process them.
                    //
                    if (!read_data(it->first))
                    {
                        auto const str = fmt::format("Connection with the client socket id = {} on '{}:{}' was closed.", 
                            it->first, ::inet_ntoa(it->second.sin_addr), ::ntohs(it->second.sin_port));
                        LNOTICE(str)
                        NOUT(str)

                        //
                        //  The connection was lost or the error happened - close the connection and remove socket
                        //
                        std::scoped_lock lock{ socket_guard_ };
                        ::close(it->first);
                        it = client_sockets_.erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }
    }
    
    close_socket_descriptors(); 
}


//
// Waits for events on the socket(s): new client connected and data obtained to read.
//
int TCPServerSocket::wait_for_events(fd_set& sockets_to_read) noexcept
{
    timeval timeout{};
    timeout.tv_sec = base::sockets::SECONDS_TO_WAIT;
    timeout.tv_usec = 0l;

    FD_ZERO(&sockets_to_read);
    FD_SET(server_socket_id_, &sockets_to_read);
    int max_id = server_socket_id_;

    {
        //std::shared_lock lock{ socket_guard_ };
        for (auto const [client_socket_id, client_address] : client_sockets_)
        {
            FD_SET(client_socket_id, &sockets_to_read);
            if (client_socket_id > max_id) max_id = client_socket_id;
        }
    }

    try
    {
        auto const rc = ::select(max_id + 1, &sockets_to_read, nullptr, nullptr, &timeout);
        if (rc == -1)
        {
            auto const str = fmt::format("Error happened while waiting data on server socket: {}.", ::strerror(errno));
            LERROR(str)
            EOUT(str)
        }
#if defined (DEVELOPER_VERSION)        
        else if (rc == 0)
        {
            NOUT("Timeout happened while waiting data on server socket.");
        }
#endif        
        return rc;
    }
    catch (const std::exception& e)
    {
        auto const str = fmt::format("Error happened while waiting data on server socket: {}.", e.what());
        LERROR(str)
        DOUT(_ERR_TEXT_ << ACTOR_TYPE  << ": " << str)
        return -1;
    }
   
}


//
//  Accepts the new client connection and creates corresponding client socket.
//
void TCPServerSocket::accept_client() noexcept
{
    sockaddr_in client_address{};
    socklen_t socket_size  = sizeof(client_address);
    ::memset(&client_address, 0, socket_size);

    int const client_socket_id = ::accept(server_socket_id_, reinterpret_cast<sockaddr*>(&client_address), &socket_size);

    if (client_socket_id < 0)
    {
        auto const str = fmt::format("Error happened while establishing connection with client: {}.", ::strerror(errno));
        LERROR(str)
        EOUT(str)
    }
    else
    {
        auto const str = fmt::format("Established connection with the client socket id = {} on '{}:{}'.", 
            client_socket_id, ::inet_ntoa(client_address.sin_addr), ::ntohs(client_address.sin_port));
        LNOTICE(str)
        NOUT(str)
  
        ::fcntl(client_socket_id, F_SETFL, O_NONBLOCK);

        std::scoped_lock lock{ socket_guard_ };
        client_sockets_[client_socket_id] = std::move(client_address);
    }
}


///
///  Initializes TCP/IP server socket and binds to the port.
///
std::pair<int, std::optional<std::string>> TCPServerSocket::initialize_socket(in_addr_t address, in_port_t port_number) noexcept
{
    std::scoped_lock lock{ socket_guard_ };

    //
    //  Create and initialize the socket
    //
    if (is_closed())
    {
        if (server_socket_id_= ::socket(AF_INET, SOCK_STREAM, 0); server_socket_id_ == INVALID_SOCKET)
        {
            return std::make_pair(errno, std::make_optional(fmt::format("Cannot create TCP/IP socket: {}.", strerror(errno))));
        }
        ::fcntl(server_socket_id_, F_SETFL, O_NONBLOCK);
        is_closed_.store(false);
    }

    //
    //  Initialize server socket address data structure.
    //
    ::memset(&server_address_, 0, sizeof(server_address_));
    server_address_.sin_family = AF_INET;
    server_address_.sin_addr.s_addr = address;
    server_address_.sin_port = port_number;

    //
    //  Bind socket to the specified port:
    //
    if (::bind(server_socket_id_, reinterpret_cast<sockaddr*>(&server_address_), sizeof(server_address_)) != base::sockets::SUCCESS ||
        ::listen(server_socket_id_, base::sockets::MAX_CLIENTS) != base::sockets::SUCCESS)
    {
        return std::make_pair(errno, std::make_optional(fmt::format("Cannot bind TCP/IP socket to '{}:{}': {}.", 
            ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port), ::strerror(errno))));
    }

    return std::make_pair(base::sockets::SUCCESS, std::nullopt);
}


///
/// Starts listener thread for messages and client connections.
///
void TCPServerSocket::start_listener() noexcept
{
    stop_listener();
    is_connected_.store(true);
    if (use_internal_thread_)
    {
        NOUT("Internal thread is used for listening.")
        listener_ = std::make_unique<std::thread>(&TCPServerSocket::process_events, this);
    }
    else
    {
        NOUT("External thread is used for listening.")
        process_events();
    }
}