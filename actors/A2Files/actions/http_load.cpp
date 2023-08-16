////////////////////////////////////////////////////////////////////////////////////////////
/** 
 
 	\brief		Загрузка произвольного файла
	\details


	--------------------------------------------
	ROUT: /http/load
	--------------------------------------------

	--------------------------------------------
	INPUT MESSAGE:
	--------------------------------------------
  
	{
		"data":
		{
			"source": <string: file path>,
			"ssl": 
			{
				"certificate": <string: path to cert>,
				"private_key": <string: path to key>,
				"no_check": <bool>
			},
			"save":
			{
				"path": <string>,
				"name":
			}

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
			"file": <string>
		}
	}

	404

	{
		"status": "error",
		"error"
		{
			"code": <int>,
			"log": <string>
		}
	}

*/
////////////////////////////////////////////////////////////////////////////////////////////


std::string A2Files::http_load(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	std::string _STATUS_ = "ok";
	/////////////////////////////////////////////////////////////////////////////////////

	std::filesystem::path source(message->data["data"]["source"].get<std::string>());
	std::string uuid = tegia::random::uuid();
	std::string filename = "/tmp/" + uuid + source.extension().string();
	
	/*
	std::cout << message->data["data"]["source"].get<std::string>() << std::endl;
	std::cout << source.string() << std::endl;
	std::cout << source.extension().string() << std::endl;
	std::cout << filename << std::endl;
	*/

	//
	// Прописываем ssl-сертификат, если он есть
	//

	auto request = core::http2::default_get_request();

	if(message->data["data"].contains("ssl") == true)
	{
		request["ssl"] = message->data["data"]["ssl"];
	}
	
	//
	// Скачиваем файл
	//

	auto http = new core::http2();

	bool flag = true;
	while(flag)
	{
		http->buffer.clear();
		http->header.clear();

		auto status = http->file(source.string(),filename,request);
		switch(status)
		{
			//
			// libCURL ERRORS
			//

			case 6:		// CURLE_COULDNT_RESOLVE_HOST
			case 7:		// CURLE_COULDNT_CONNECT
			case 18:	// CURLE_PARTIAL_FILE
			case 28:	// CURLE_OPERATION_TIMEDOUT
			{
				std::cout << _ERR_TEXT_ << "A2Files::http_load " << status << "\n";
				std::this_thread::sleep_for(500ms);
			}
			break;

			// case 52:	// CURLE_GOT_NOTHING (52)  ??? 

			//
			// HTTP ERRORS
			//

			case 200:
			{
				/*
				std::string filename = "/tmp/" + uuid + source.extension().string();

				std::ofstream file;
				file.open(filename.c_str(), std::ios::binary);
				file.write(http->buffer.c_str(),http->buffer.size());
				file.close();
				*/

				message->data["status"] = "ok";
				message->data["data"]["file"] = filename;
			
				_STATUS_ =  "200";
				flag = false;	
			}
			break;

			case 302:
			{
				auto jheader = http->header2json(http->header);
				//std::cout << jheader.dump() << std::endl;

				if(http->_data.headers.contains("location") == true)
				{
					std::cout << "    [302] " << http->_data.headers["location"] << "\n";
					source = std::filesystem::path(http->_data.headers["location"]);
					break;
				}

				if(http->_data.headers.contains("Location") == true)
				{
					std::cout << "    [302] " << http->_data.headers["Location"] << "\n";
					source = std::filesystem::path(http->_data.headers["Location"]);
					break;
				}

				std::cout << _ERR_TEXT_ << "[302] not found 'location'\n";
				std::cout << http->_data.raw_headers;
				exit(0);
			}
			break;

			case 404:
			{
				message->data["status"] = "error";
				message->data["error"]["code"] = 404;
				
				_STATUS_ =  "404";	
				flag = false;
			}
			break;

			default:
			{
				std::cout << _ERR_TEXT_ << status << "\n";
				std::cout << http->header << "\n";
				exit(0);
			}
			break;
		}
	}	// END while(flag)

	delete http;

	/////////////////////////////////////////////////////////////////////////////////////
	return _STATUS_;
};   

