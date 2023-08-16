#include <cstdlib>
#include <string>

#include "../Constants.h"
#include <tegia/sockets/Helper.h>

////////////////////////////////////////////////////////////////////////////////////////////
/** 

	--------------------------------------------
	ROUT: /unzip
	--------------------------------------------

	--------------------------------------------
	INPUT MESSAGE:
	--------------------------------------------
  
	{
		"data":
		{
			"file": <string: file path>
			"folder": <string: folder to unzip data>
		}
	}

	--------------------------------------------
	OUTPUT MESSAGE:
	--------------------------------------------
	
	200

	{
		"status": "ok",
		"data"
		{
			"file": <string>,
			"folder": <string>
		}
	}

	ERROR

	{
		"status": "error",
		"error"
		{
			"code": <int>
			"info": <string>
		}
	}


https://www.linux.org.ru/forum/development/726776


*/   
////////////////////////////////////////////////////////////////////////////////////////////

using namespace base;

std::string A2Files::unzip(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params) const
{
	const auto process_error = [&message] (const int error_code, std::string&& error_str) -> std::string
	{
        LERROR(error_str)
	    message->data[ERROR_STATUS_FIELD] = ERROR_STATUS;
        message->data["error"][ERROR_CODE_FIELD] = error_code;        
        message->data["error"][ERROR_INFO_FIELD] = std::move(error_str);
        return ERROR_STATUS;
	};

	if (!message->data.contains("data") || !message->data["data"].contains(UNZIP_FILE_NAME))
	{
		return process_error(BAD_REQUEST_ERROR, std::string("File name to unzip isn't specified!"));
	} 
	const std::string& file_name = message->data["data"][UNZIP_FILE_NAME];

	if (!message->data["data"].contains(UNZIP_FOLDER_NAME)) 
	{
		message->data["data"][UNZIP_FOLDER_NAME] = "/tmp/" + file_name.substr(0U, file_name.rfind('.'));
	}
	const std::string& folder = message->data["data"][UNZIP_FOLDER_NAME];

	int sys{};

	if (tegia::common::ends_with_ic(file_name, ".bz2"))
	{
		sys = system(("bzip2 \"" + file_name + "\" -d \"" + folder + '\"').c_str());
	}
	else
	{
		sys = system(("unzip -q -x -o \"" + file_name + "\" -d \"" + folder + '\"').c_str());
	}

	if (message->data["data"].contains(UNZIP_DELETE_FLAG) && message->data["data"][UNZIP_DELETE_FLAG].get<bool>())
	{
		std::filesystem::remove(file_name);
	}

	if (sys != 0)
	{
		//std::cout << "Unzip utility returned error code = " << sys << ".\n";
		return process_error(sys, fmt::format("Problem unzipping file '{}'. Unzip utility returned non-zero code = {}.", file_name, sys));
	}

	message->data[ERROR_STATUS_FIELD] = OK_STATUS;
	return OK_STATUS;
}; 

