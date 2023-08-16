#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string>
#include <tuple>
#include <optional>

#include <nlohmann/json-schema.hpp>

#include "Constants.h"
#include <tegia/sockets/Helper.h>

using socket_address = std::pair<std::optional<std::string>, std::optional<in_port_t>>;

///
/// Gets IP address and port number from the input JSON data.
///
socket_address get_socket_address(nlohmann::json const& input_data) noexcept
{
    socket_address rc { std::nullopt, std::nullopt };

    if (input_data.contains(base::sockets::HOST_ADDRESS) && input_data[base::sockets::HOST_ADDRESS].is_string())
    {
        auto host_address = input_data[base::sockets::HOST_ADDRESS].get<std::string>();
        if (!host_address.empty())
        {
            rc.first = std::move(host_address);
        }
    }

    if (input_data.contains(base::sockets::PORT_NUMBER) && !input_data[base::sockets::PORT_NUMBER].is_null()) 
    {
        if (input_data[base::sockets::PORT_NUMBER].is_number_unsigned())
        {
            rc.second = input_data[base::sockets::PORT_NUMBER].get<in_port_t>();
        }
        else if (input_data[base::sockets::PORT_NUMBER].is_string())
        {
            rc.second = tegia::common::try_parse_remove_leading_blanks<in_port_t>(input_data[base::sockets::PORT_NUMBER].get<std::string>());
        }
    }

    return rc;

}

