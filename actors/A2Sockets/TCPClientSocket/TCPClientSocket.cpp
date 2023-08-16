#include "TCPClientSocket.h"
#include <tegia/sockets/Helper.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONSTRUCTOR / DESTRUCTOR                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


TCPClientSocket::TCPClientSocket(std::string const& name, nlohmann::json& data) 
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

TCPClientSocket::~TCPClientSocket() noexcept
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
ssize_t TCPClientSocket::read_n_bytes(void* const buf, std::size_t const N) const
{
    std::size_t offset = 0U;
    while (is_connected()) 
    {
       ssize_t const rc = ::recv(client_socket_id_, reinterpret_cast<void*>(reinterpret_cast<char*>(buf) + offset), N - offset, MSG_DONTWAIT);
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


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "actions/connect.cpp"
#include "actions/stop.cpp"
#include "actions/send.cpp"