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

#define ACTOR_TYPE "TCPClientSocket"

//#define DEVELOPER_VERSION

#if defined(DEVELOPER_VERSION)
#define DOUT(text) std::cout << text << std::endl;     
#else
#define DOUT(text)     
#endif

#define EOUT(text) DOUT(_ERR_TEXT_ << ACTOR_TYPE  << ": " << text)
#define NOUT(text) DOUT(_OK_TEXT_ << ACTOR_TYPE  << ": " << text)


using error_data = std::pair<int, std::optional<std::string>>;


class TCPClientSocket final : public tegia::actors::actor_base
{	
  public:

    TCPClientSocket(std::string const& name, nlohmann::json& data, bool const use_internal_thread) : TCPClientSocket(name, data)
    {
       use_internal_thread_ = use_internal_thread;
    }
    TCPClientSocket(std::string const& name, nlohmann::json& data);
    ~TCPClientSocket() noexcept override;


    TCPClientSocket(TCPClientSocket const&) = delete;
    TCPClientSocket(TCPClientSocket&&) noexcept = delete;
    TCPClientSocket& operator = (TCPClientSocket const&) = delete;
    TCPClientSocket& operator = (TCPClientSocket&&) noexcept = delete;

    // ----------------------------------------------------------------------------------
    // ACTION FUNCTIONS
    // ----------------------------------------------------------------------------------

    std::string connect(std::shared_ptr<message_t> const& message,    nlohmann::json const& route_params) noexcept;
    std::string disconnect(std::shared_ptr<message_t> const& message, nlohmann::json const& route_params) noexcept;
    std::string send(std::shared_ptr<message_t> const& message,       nlohmann::json const& route_params) noexcept;

    // ----------------------------------------------------------------------------------
    // API FUNCTIONS
    // ----------------------------------------------------------------------------------

    bool is_connected() const noexcept { return is_connected_.load(); }
    bool is_closed() const noexcept { return is_closed_.load(); }

    error_data start_socket(std::string&& ip_address, in_port_t port_number) noexcept;
    error_data close_socket() noexcept;
    error_data send_message(std::string_view message_body) noexcept;


  protected:

  private:

    std::atomic<bool> is_connected_ = false;
    std::atomic<bool> is_closed_ = true;
    std::mutex socket_guard_{};

    sockaddr_in server_address_{};
    int client_socket_id_ = base::sockets::INVALID_SOCKET;

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
    error_data close_socket_descriptor() noexcept;
    void stop_listener() noexcept;
    void start_listener() noexcept;
    void receive_messages() noexcept;
    bool wait_for_event() noexcept;
    bool read_data() noexcept;
    ssize_t read_n_bytes(void* buf, std::size_t N) const;
 
  };


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


extern "C" tegia::actors::router_base* _load_actor()
{
	auto ro = new tegia::actors::router<TCPClientSocket>(ACTOR_TYPE);

	ACTION_ROUTE2("/TCPClientSocket/start/{host}/{port}",   &TCPClientSocket::connect);
	ACTION_ROUTE2("/TCPClientSocket/connect",               &TCPClientSocket::connect);
	ACTION_ROUTE2("/TCPClientSocket/stop",                  &TCPClientSocket::disconnect);
	ACTION_ROUTE2("/TCPClientSocket/send",                  &TCPClientSocket::send);
	ACTION_ROUTE2("/TCPClientSocket/post/{message}",				&TCPClientSocket::send);

	return ro;
};