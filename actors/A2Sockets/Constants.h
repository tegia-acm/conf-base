#ifndef EC47F1F8_47A3_40D5_B764_5FEA29A86BF0
#define EC47F1F8_47A3_40D5_B764_5FEA29A86BF0

#pragma once

namespace base::sockets
{
    using namespace std::literals::string_literals;

    inline constexpr int INVALID_SOCKET = -1;
    inline constexpr int SUCCESS = 0;

    inline constexpr int BAD_REQUEST_ERROR = 400;
    inline constexpr int NOT_FOUND_ERROR = 404;
    inline constexpr int BAD_FORMAT_ERROR = 415;
    inline constexpr int INTERNAL_SERVER_ERROR = 500;
    inline constexpr int SERVICE_INAVAILABLE_ERROR = 503;
    inline constexpr int UNKNOWN_ERROR = 520;

    inline constexpr int DB_SUCCESS_CODE = 200;

    inline constexpr int MAX_CLIENTS = 4;
    inline constexpr __time_t SECONDS_TO_WAIT = 10L;
    inline constexpr std::size_t MESSAGE_BUFFER_SIZE = 4096U;
    inline constexpr std::size_t MAXIMAL_BUFFER_SIZE = MESSAGE_BUFFER_SIZE * MESSAGE_BUFFER_SIZE;
    inline constexpr char TEGIA_SIGNATURE[] = "TEG_SIG#";

    inline const std::string OK_STATUS      = "ok"s;
    inline const std::string ERROR_STATUS   = "error"s;

    inline const std::string RETURN_CODE    = "status"s;
    inline const std::string RETURN_INFO    = "info"s;
    inline const std::string HOST_ADDRESS   = "host"s;
    inline const std::string PORT_NUMBER    = "port"s;
    inline const std::string TCPIP_MESSAGE  = "message"s;

}


#endif /* EC47F1F8_47A3_40D5_B764_5FEA29A86BF0 */
