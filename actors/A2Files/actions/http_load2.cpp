////////////////////////////////////////////////////////////////////////////////////////////
/** 
 
 	\brief		Загрузка произвольного файла
	\details

	"task":
 	{
		"uuid": <uuid>,
		"type": "HTTP_LOAD",
		"data":
		{
			"url": <string>,
			"filename": <string>,
			"ssl":
			{
				"cert": <string>,
				"password": <string>,
				"is_check": <bool>
			}
		}
 	}

	status:		
		400 - ошибка валидации данных

*/
////////////////////////////////////////////////////////////////////////////////////////////


std::string A2Files::http_load2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	std::string _STATUS_ = "ok";
	/////////////////////////////////////////////////////////////////////////////////////

	try
	{
		this->http_load_validator.validate(message->data);
	}

	catch(const std::exception& e)
	{
		std::cout << _ERR_TEXT_ << e.what() << '\n';

		// TODO:

		message->data["status"] = 400;
		message->data["error"]["code"] = 1;
		message->data["error"]["info"] = e.what();

		message->data["task"]["status"] = 400;
		message->data["task"]["error"]["code"] = 1;
		message->data["task"]["error"]["info"] = e.what();
		
		return "400";
	}

	// std::cout << message->data << std::endl;

	std::string url = message->data["task"]["data"]["url"].get<std::string>();
	std::string filename = message->data["task"]["data"]["filename"].get<std::string>();

	std::filesystem::path file(filename);
	std::filesystem::path dir = file.parent_path();
	std::filesystem::path tmp_file(dir / std::string(tegia::random::uuid() + ".tmp"));

	//
	// Проверить существование файла, который необходимо скачать
	// 


	if(std::filesystem::is_regular_file(file) == true)
	{
		// TODO: какое должно быть поведения?

		message->data["status"] = 200;
		message->data["error"]["code"] = 1;
		message->data["error"]["info"] = "file [" + filename + "] is already exists";

		message->data["task"]["status"] = 200;
		message->data["task"]["error"]["code"] = 1;
		message->data["task"]["error"]["info"] = "file [" + filename + "] is already exists";

		std::cout << _ERR_TEXT_ << "file [" << filename << "] is already exists" << std::endl;

		//
		// READ FILE SIZE
		//

		std::error_code ec;
		std::uintmax_t size = std::filesystem::file_size(file, ec);
		message->data["task"]["data"]["size"] = size;

		return "200";
	}

	//
	// Проверить существование каталога
	//

	// std::cout << "dir = " << dir.string() << std::endl; 

	if(std::filesystem::is_directory(dir) == false)
	{
		std::filesystem::create_directories(dir);
	}

	auto http = std::make_shared<tegia::http::client>();

	//
	// Настроить SSL
	//

	if(message->data.contains("/task/data/ssl/cert"_json_pointer) == true)
	{
		http->set_sslp12(
			message->data["/task/data/ssl/cert"_json_pointer].get<std::string>(),
			message->data["/task/data/ssl/password"_json_pointer].get<std::string>(),
			message->data["/task/data/ssl/is_check"_json_pointer].get<bool>()
		);
	}

	bool flag = true;

	while(flag)
	{
		int status = http->get(url,tmp_file.string()); //filename);
		switch(status)
		{
			case 200:
			{
				std::filesystem::rename(tmp_file,file);

				message->data["status"] = 0;
				message->data["task"]["data"]["size"] = core::cast<long long int>(http->response->headers["content-length"]);
				return "ok";
			}
			break;

			case 6:		// CURLE_COULDNT_RESOLVE_HOST
			case 7:		// CURLE_COULDNT_CONNECT
			case 18:	// CURLE_PARTIAL_FILE
			case 28:	// CURLE_OPERATION_TIMEDOUT
			case 35:	// CURLE_SSL_CONNECT_ERROR
			{
				std::this_thread::sleep_for(1s);

				// TODO: механизм отложенных сообщений!
			}
			break;

			default:
			{
				std::cout << _ERR_TEXT_ << "[" << status << "] " << url << std::endl;
				exit(0);
			}
			break;
		}
	}	// while(flag)

	/////////////////////////////////////////////////////////////////////////////////////
	return _STATUS_;
};   

