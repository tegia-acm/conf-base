#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	C++ STL
#include <memory>
#include <map>
#include <string>
#include <string_view>
#include <atomic>
#include <chrono> 
#include <mutex>
//#include <shared_mutex>
#include <optional>
#include <tuple>
#include <thread>

// Socket support
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>

//	TEGIA PLATFORM
#include <tegia/tegia.h>
#include <tegia/actors/routers_map.h>
#include <tegia/actors/actor_base.h>
#include <tegia/core/core.h> 
	
//	VENDORS 
#include <nlohmann/json-schema.hpp>
#include <fmt/core.h>

//  Common project constants and methods
#include "../Constants.h"

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTOR CLASS                                                                            //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define ACTOR_TYPE "TCPServerSocket"

//#define DEVELOPER_VERSION

#if defined(DEVELOPER_VERSION)
#define DOUT(text) std::cout << text << std::endl;     
#else
#define DOUT(text)     
#endif

#define EOUT(text) DOUT(_ERR_TEXT_ << ACTOR_TYPE  << ": " << text)
#define NOUT(text) DOUT(_OK_TEXT_ << ACTOR_TYPE  << ": " << text)


using error_data = std::pair<int, std::optional<std::string>>;


class TCPServerSocket final: public tegia::actors::actor_base
{	
  public:

    TCPServerSocket(std::string const& name, nlohmann::json& data, bool const use_internal_thread) : TCPServerSocket(name, data)
    {
       use_internal_thread_ = use_internal_thread;
    }
    TCPServerSocket(std::string const& name, nlohmann::json& data);
    ~TCPServerSocket() noexcept override;


    TCPServerSocket(TCPServerSocket const&) = delete;
    TCPServerSocket(TCPServerSocket&&) noexcept = delete;
    TCPServerSocket& operator = (TCPServerSocket const&) = delete;
    TCPServerSocket& operator = (TCPServerSocket&&) noexcept = delete;

    // ----------------------------------------------------------------------------------
    // ACTION FUNCTIONS
    // ----------------------------------------------------------------------------------

    std::string loop(std::shared_ptr<message_t> const& message, nlohmann::json const& route_params) noexcept;
    std::string stop(std::shared_ptr<message_t> const& message, nlohmann::json const& route_params) noexcept;
    std::string send(std::shared_ptr<message_t> const& message, nlohmann::json const& route_params) noexcept;

    // ----------------------------------------------------------------------------------
    // API FUNCTIONS
    // ----------------------------------------------------------------------------------

    bool is_connected() const noexcept { return is_connected_.load(); }
    bool is_closed() const noexcept { return is_closed_.load(); }

    error_data start_loop(std::string&& ip_address, in_port_t port_number) noexcept;
    error_data close_socket() noexcept;
    error_data send_message(in_addr ip_address, in_port_t port_number, std::string_view message_body) noexcept;

  protected:

  private:

    std::atomic<bool> is_connected_ = false;
    std::atomic<bool> is_closed_ = true;
    std::mutex socket_guard_{};

    sockaddr_in server_address_{};
    int server_socket_id_ = base::sockets::INVALID_SOCKET;

    std::map<int, sockaddr_in> client_sockets_{};

    std::unique_ptr<std::thread> listener_ = nullptr;

    void* send_buffer = nullptr;
    std::size_t send_buffer_size = 0U;
    void* read_buffer = nullptr;
    std::size_t read_buffer_size = 0U;

    bool use_internal_thread_ = true;

    // ----------------------------------------------------------------------------------   
    // SUPPORT FUNCTIONS 
    // ----------------------------------------------------------------------------------   

    error_data initialize_socket(in_addr_t address, in_port_t port_number) noexcept;
    void accept_client() noexcept;
    void start_listener() noexcept;

    int wait_for_events(fd_set& sockets_to_read) noexcept;
    void process_events() noexcept;

    error_data close_socket_descriptors() noexcept;
    void stop_listener() noexcept;

    ssize_t read_n_bytes(int socket_id, void* buf, std::size_t N) const;
    bool read_data(int socket_id) noexcept;
};


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


extern "C" tegia::actors::router_base* _load_actor()
{
	auto ro = new tegia::actors::router<TCPServerSocket>(ACTOR_TYPE);

	ACTION_ROUTE2("/TCPServerSocket/start/{host}/{port}",   		    &TCPServerSocket::loop);
	ACTION_ROUTE2("/TCPServerSocket/loop",                  		    &TCPServerSocket::loop);
	ACTION_ROUTE2("/TCPServerSocket/stop",                  		    &TCPServerSocket::stop);
	ACTION_ROUTE2("/TCPServerSocket/send",                  		    &TCPServerSocket::send);
	ACTION_ROUTE2("/TCPServerSocket/post/{host}/{port}/{message}",	&TCPServerSocket::send);

	return ro;
};