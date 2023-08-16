#pragma once

namespace base
{
    using namespace std::literals::string_literals;

    inline const std::string OK_STATUS = "200"s;
    inline const std::string ERROR_STATUS = "error"s;

    inline const std::string ERROR_STATUS_FIELD = "status"s;
    inline const std::string ERROR_CODE_FIELD = "code"s;
    inline const std::string ERROR_INFO_FIELD = "info"s;

    inline constexpr int BAD_REQUEST_ERROR = 400;

    inline const std::string UNZIP_FILE_NAME = "file"s;
    inline const std::string UNZIP_FOLDER_NAME = "folder"s;
    inline const std::string UNZIP_DELETE_FLAG = "delete_file"s;
}
