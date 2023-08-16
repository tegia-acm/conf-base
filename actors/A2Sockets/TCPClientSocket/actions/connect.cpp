using namespace base::sockets;

///
///  Creates TCP/IP client socket, connects to server socket and waits in non-blocking loop for the data sent.
///
std::string TCPClientSocket::connect(std::shared_ptr<message_t> const& message, nlohmann::json const& route_params) noexcept
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
        output_data["error"][RETURN_INFO] = std::move(error_str);
        return ERROR_STATUS;
    };

    //
    //  Get port number and server IP address from the input data:
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
        = start_socket(server_socket_address.first.has_value() ? std::move(*server_socket_address.first) : std::string{}, *server_socket_address.second);
    if (error_code != SUCCESS) 
    {
       return process_error(error_code, error_string.has_value() ? std::move(*error_string) : std::string("Unspecified error happened."));

    }

    output_data[RETURN_INFO] = error_string.has_value() ? std::move(*error_string) : std::string("TCP/IP client socket was connected to the server.");
    LNOTICE(*output_data[RETURN_INFO].get_ptr<const nlohmann::json::string_t*>())
    NOUT(*output_data[RETURN_INFO].get_ptr<const nlohmann::json::string_t*>())

    output_data[RETURN_CODE] = OK_STATUS;
    return OK_STATUS;
}


//
// Creates client TCP/IP socket, connects it to the server and starts listening.
//
error_data TCPClientSocket::start_socket(std::string&& ip_address, in_port_t port_number) noexcept
{
    in_addr address{};
    address.s_addr = ::htonl(INADDR_LOOPBACK);
    ::inet_aton(ip_address.c_str(), &address);
    port_number = ::htons(port_number);
    
    //
    // Check if socket is already initialized and running:
    //
    if (is_connected())
    {
        //
        //  If it is connected to the same server then it's nothing to do.
        //
        if (address.s_addr == server_address_.sin_addr.s_addr && port_number == server_address_.sin_port)
        {
            return std::make_pair(SUCCESS, std::make_optional(fmt::format("TCP/IP client socket connection with server '{}:{}' it is already established.", 
                ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port))));
        }
        //
        //  If it is connected to another server then close existing connection first:
        //
        if (auto rc = close_socket(); rc.first != SUCCESS)  return std::move(rc);
    }
    
    //
    //  Create socket and try to connect to the server:
    //
    if (auto rc = initialize_socket(address.s_addr, port_number); rc.first != SUCCESS)
    {
        return rc;
    }

    //
    // Starts listening for server messages
    //
    start_listener();

    return std::make_pair(base::sockets::SUCCESS, fmt::format("TCP/IP client socket with id = {} is connected to the server '{}:{}'.", 
        client_socket_id_, ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port)));
}


///
///  Initializes TCP/IP client socket and tries to connect to the server.
///
std::pair<int, std::optional<std::string>> TCPClientSocket::initialize_socket(in_addr_t const address, in_port_t const port_number) noexcept
{
    std::scoped_lock lock{ socket_guard_ };

    //
    //  Create and initialize socket
    //
    if (is_closed())
    {
        if (client_socket_id_= ::socket(AF_INET, SOCK_STREAM, 0); client_socket_id_ == INVALID_SOCKET)
        {
            return std::make_pair(errno, std::make_optional(fmt::format("Cannot create TCP/IP socket: {}.", strerror(errno))));
        }
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
    //  Connect socket to the server
    //
    if (::connect(client_socket_id_, reinterpret_cast<sockaddr*>(&server_address_), sizeof(server_address_)) != SUCCESS)
    {
        return std::make_pair(errno, std::make_optional(fmt::format("Cannot connect TCP/IP socket to server '{}:{}': {}.", 
            ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port), ::strerror(errno))));
    }
   
    return std::make_pair(base::sockets::SUCCESS, std::nullopt);
}


///
/// Starts listener thread for server messages and sert the flag.
///
void TCPClientSocket::start_listener() noexcept
{
    stop_listener();
    is_connected_.store(true);
    if (use_internal_thread_)
    {
        NOUT("Internal thread is used for message listening.")
        listener_ = std::make_unique<std::thread>(&TCPClientSocket::receive_messages, this);
    }
    else
    {
        NOUT("External thread is used for message listening.")
        receive_messages();
    }
}

///
/// Waits until any event happens on the socket and process it
//
void TCPClientSocket::receive_messages() noexcept
{
    //
    //  Loop waiting for the new data sent from the server:
    //
    while (is_connected())
    {
        if (wait_for_event())
        {
            //
            // Obtained data from the server. Try to read and process them.
            //
            if (!read_data())
            {
                is_connected_.store(false);
                auto [error_code, error_string] = close_socket_descriptor();
                if (!error_string.has_value())
                { 
                  error_string = std::string("Connection with the server socket was closed.");
                }
                LNOTICE(*error_string)
                NOUT(*error_string)
            }
        }
    }
}


//
// Waits for event on the socket: new data obtained to read.
//
bool TCPClientSocket::wait_for_event() noexcept
{
    timeval timeout{};
    timeout.tv_sec = SECONDS_TO_WAIT;
    timeout.tv_usec = 0L;

    fd_set socket_to_read{};
    FD_ZERO(&socket_to_read);
    FD_SET(client_socket_id_, &socket_to_read);

    if (!is_connected()) return false;
    if (auto const rc = ::select(client_socket_id_ + 1, &socket_to_read, nullptr, nullptr, &timeout); rc < 1)
    {
        if (rc == -1)
        {
            auto const str = fmt::format("Error happened while waiting data on client socket with id = {}: {}.", client_socket_id_, ::strerror(errno));
            LERROR(str)
            EOUT(str)
        }
#if defined (DEVELOPER_VERSION)        
        else if (rc == 0)
        {
            NOUT(fmt::format("Timeout happened while waiting data on client socket with id = {}.", client_socket_id_));
        }
#endif        
        return false;
    }
    return true;
}


///
/// Reads message data from the client socket, parses them and perform the required action:
///
bool TCPClientSocket::read_data() noexcept
{
    constexpr size_t sign_len = std::strlen(TEGIA_SIGNATURE);
    char signature[sign_len]{};

    try
    {
        //
        //  Get first 'sig_len' bytes to read message signature
        //
        auto len = read_n_bytes(reinterpret_cast<void*>(signature), sign_len);
        if (len > 0L)
        {
            if (static_cast<std::size_t>(len) == sign_len)
            {
                //
                //  Check that is the correct TEGIA message format
                //  
                if (tegia::common::equal_ic(std::string_view(TEGIA_SIGNATURE, sign_len), std::string_view(signature, sign_len)))
                {
                    //
                    //  Get the length of message body
                    //
                    std::uint64_t msg_len{};
                    len = read_n_bytes(reinterpret_cast<void*>(&msg_len), sizeof(msg_len));
                    if (static_cast<std::size_t>(len) == sizeof(msg_len))
                    {
                        // 
                        // Allocate the required buffer:
                        //
                        if (msg_len > read_buffer_size)
                        {
                            if (msg_len <= MAXIMAL_BUFFER_SIZE)
                            {
                                if (void* const new_buffer = ::realloc(read_buffer, msg_len); new_buffer != nullptr)
                                {
                                    read_buffer = new_buffer;
                                    read_buffer_size = msg_len;
                                }
                            } 
                            if (msg_len > read_buffer_size)
                            {
                                do
                                {
                                    len = ::recv(client_socket_id_, read_buffer, read_buffer_size, MSG_DONTWAIT);
                                } 
                                while (static_cast<std::size_t>(len) == read_buffer_size);
                                auto const str = fmt::format("Cannot read TCP/IP message from the server socket on '{}:{}'. Not enough memory is available.", 
                                    ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port));
                                LWARNING(str)
                                EOUT(str)
                                NOUT(std::string_view(reinterpret_cast<char*>(read_buffer), len))
                                return true;
                            }
                        }
                        
                        // 
                        // TODO!!! Read the messaage itself:
                        //
                        //len = read_n_bytes(client_socket_id_, read_buffer, msg_len);
                        len = ::recv(client_socket_id_, read_buffer, read_buffer_size, 0);
                        if (static_cast<std::size_t>(len) != msg_len)
                        {
                            auto const str = fmt::format("Cannot read the entire TCP/IP Tegia message from the server socket on '{}:{}'. Ignored.", 
                                ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port));
                            LWARNING(str)
                            EOUT(str)
                            NOUT(std::string_view(reinterpret_cast<char*>(read_buffer), len))
                       }
                        else
                        {
                            //
                            // TODO!!! Deserialize the message and perform further actions.
                            //
                            NOUT(std::string_view(reinterpret_cast<char*>(read_buffer), len))
                        }
                    }
                }
                else
                {
                    do
                    {
                        len = ::recv(client_socket_id_, read_buffer, read_buffer_size, MSG_DONTWAIT);
                    } 
                    while (static_cast<std::size_t>(len) == read_buffer_size);
                    NOUT("Unknown message was obtained from the server on '" << ::inet_ntoa(server_address_.sin_addr) << ':' 
                        << ::ntohs(server_address_.sin_port) << "': " << std::string_view(signature, sign_len) 
                        << (len > 0L ? std::string_view(reinterpret_cast<char*>(read_buffer), len) : std::string_view(""))) 
                }
            }
           return true;
        }
    }
    catch (const std::exception& e)
    {
        auto const str = fmt::format("Error happened while reading data from the server socket on '{}:{}'. {}.", 
            ::inet_ntoa(server_address_.sin_addr), ::ntohs(server_address_.sin_port), e.what());
        LERROR(str)
        EOUT(str)
    }   

    return false;
}
