#include "TCPServerSocket.h"
#include <tegia/sockets/Helper.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONSTRUCTOR / DESTRUCTOR                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


TCPServerSocket::TCPServerSocket(std::string const& name, nlohmann::json& data) 
	: tegia::actors::actor_base(ACTOR_TYPE, name, data)
{
    if (void* const new_buffer = ::malloc(base::sockets::MESSAGE_BUFFER_SIZE); new_buffer != nullptr)
    {
        read_buffer = new_buffer;
        read_buffer_size = base::sockets::MESSAGE_BUFFER_SIZE;
    } 
    if (void* const new_buffer = ::malloc(base::sockets::MESSAGE_BUFFER_SIZE); new_buffer != nullptr)
    {
        send_buffer = new_buffer;
        send_buffer_size = base::sockets::MESSAGE_BUFFER_SIZE;
    }
}

TCPServerSocket::~TCPServerSocket() noexcept
{
	close_socket();
    std::scoped_lock lock { socket_guard_ };
    ::free(send_buffer); 
    send_buffer = nullptr;
    send_buffer_size = 0U;
    ::free(read_buffer); 
    read_buffer = nullptr;
    read_buffer_size = 0U;
}

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SUPPORT FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "../common.cpp"


///
/// Reads first 'N' bytes from the socket stream
///
ssize_t TCPServerSocket::read_n_bytes(int const socket_id, void* const buf, std::size_t const N) const
{
    std::size_t offset = 0U;
    while (is_connected()) 
    {
        ssize_t const rc = ::recv(socket_id, reinterpret_cast<void*>(reinterpret_cast<char*>(buf) + offset), N - offset, MSG_WAITALL);
        if (rc < 0L) 
        {
            // Error occurred
            if (errno == EWOULDBLOCK) return static_cast<ssize_t>(offset);
            if (errno != EINTR) return rc;
        } 
        else if (rc == 0) 
        {
            // Connection closed
            return 0L;
        } 
        else if (offset + rc == N) 
        {
            // All N bytes read
            return static_cast<ssize_t>(N);
        } 
        else 
        {
            // Continue reading
            offset += rc;
        }
    }
    return static_cast<ssize_t>(offset);
}


///
/// Reads message data from the client socket, parses them and perform the required action:
///
bool TCPServerSocket::read_data(int const socket_id) noexcept
{
    constexpr auto const sign_len = std::strlen(base::sockets::TEGIA_SIGNATURE);
    char signature[sign_len]{};
    in_port_t port{};
    in_addr ip_address{};

    try
    {
        if (client_sockets_.contains(socket_id))
        {
            port = ::ntohs(client_sockets_.at(socket_id).sin_port);
            ip_address = client_sockets_.at(socket_id).sin_addr;  
        }
        else
        {
            return true;
        }
        //
        //  Get first 'sig_len' bytes to read message signature:
        //
        auto len = read_n_bytes(socket_id, reinterpret_cast<void*>(signature), sign_len);
        if (len > 0L)
        {
            if (static_cast<std::size_t>(len) == sign_len)
            {
                //
                //  Check that is the correct TEGIA message format
                //  
                if (tegia::common::equal_ic(std::string_view(base::sockets::TEGIA_SIGNATURE, sign_len), std::string_view(signature, sign_len)))
                {
                    //
                    //  Get the length of message body
                    //
                    std::uint64_t msg_len{};
                    len = read_n_bytes(socket_id, reinterpret_cast<void*>(&msg_len), sizeof(msg_len));
                    if (static_cast<std::size_t>(len) == sizeof(msg_len))
                    {
                        // 
                        // Allocate the required buffer:
                        //
                        if (msg_len > read_buffer_size)
                        {
                            if (msg_len <= base::sockets::MAXIMAL_BUFFER_SIZE)
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
                                    len = ::recv(socket_id, read_buffer, read_buffer_size, MSG_DONTWAIT);
                                } 
                                while (static_cast<std::size_t>(len) == read_buffer_size);
                                auto const str = fmt::format("Cannot read TCP/IP message from the server socket on '{}:{}'. Not enough memory is available.", 
                                    ::inet_ntoa(ip_address), port);
                                LWARNING(str)
                                EOUT(str)
                                NOUT(std::string_view(reinterpret_cast<char*>(read_buffer), len))
                                return true;
                            }
                        }

                        // 
                        // Read the messaage itself:
                        //
                        len = read_n_bytes(socket_id, read_buffer, msg_len);
                        if (static_cast<std::size_t>(len) != msg_len)
                        {
                            auto const str = fmt::format("Cannot read the entire TCP/IP Tegia message from the client socket on '{}:{}'. Ignored.", 
                                ::inet_ntoa(ip_address), port);
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
                    else
                    {
                        auto const str = fmt::format("Invalid format of TCP/IP Tegia message was obtained from the client socket on '{}:{}'. Ignored.", 
                            ::inet_ntoa(ip_address), port);
                        LWARNING(str)
                        EOUT(str)
                    }
                }
                else
                {
                    //
                    // Just read unknown message to remove data from socket buffer.
                    //
                    do
                    {
                        len = ::recv(socket_id, read_buffer, read_buffer_size, MSG_DONTWAIT);
                    } 
                    while (static_cast<std::size_t>(len) == read_buffer_size);
                    NOUT("Unknown message was obtained from the client on '" << ::inet_ntoa(ip_address) << ':' << port << "': " 
                        << std::string_view(signature, sign_len) << (len > 0L ? std::string_view(reinterpret_cast<char*>(read_buffer), len) : std::string_view(""))) 
                }
            }
            else
            {
                NOUT("Unknown message was obtained from the client on '" << ::inet_ntoa(ip_address) << ':' << port << "': " << std::string_view(signature, len)) 
            }
            return true;
        }
    }
    catch (const std::exception& e)
    {
        auto const str = fmt::format("Error happened while reading data from the client socket on '{}:{}'. {}.", ::inet_ntoa(ip_address), port, e.what());
        LERROR(str)
        EOUT(str)
    }   

    return false;
}


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "actions/loop.cpp"
#include "actions/stop.cpp"
#include "actions/send.cpp"
