/////////////////////////////////////////////////////////////////////////////////////////
/*

	"task":
	{
		"uuid": <uuid>,
		"type": "EMAIL_SEND",
		"data":
		{
			"from": <string>,
			"to": <string>,
			"subject": <string>,
			"template": "mychek-order.tmpl",
			"personalizations":
			{
				<data object>
			}
		}
	}

*/
/////////////////////////////////////////////////////////////////////////////////////////

std::string A2Mailer::send(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	/////////////////////////////////////////////////////////////////////////////////////

	//
	// Валидируем формат задачи
	//
	
	try
	{
		this->email_task_validator.validate(message->data);
	}

	catch(const std::exception& e)
	{
		message->data["task"]["status"] = 400;
		message->data["task"]["error"]["code"] = 1;
		message->data["task"]["error"]["info"] = e.what();

		LERROR("[400] DATA NOT VALID \n" << message->data.dump());

		return "400";
	}

	//
	// Проверяем, что для _from есть настройки в конфигурации 
	//

	std::string _from = message->data["task"]["data"]["from"].get<std::string>();
	auto pos = this->emails.find(_from);
	if(pos == this->emails.end())
	{
		message->data["task"]["status"] = 404;
		message->data["task"]["error"]["code"] = 1;
		message->data["task"]["error"]["info"] = "email '" + _from + "' configs not found";

		LERROR("[404] EMAIL NOT FOUND \n" << message->data.dump());

		return "404";
	}
	
	//
	// По шаблону определяем способ отправки
	// 

	std::string _template = message->data["task"]["data"]["template"].get<std::string>();
	auto it = this->templates.find(_template);
	if(it == this->templates.end())
	{
		// TODO: error!

		std::cout << _ERR_TEXT_ << "not found template '" + _template + "'" << std::endl;
		exit(0);
	}

	switch(tegia::crypt::crc32(it->second["type"].get<std::string>()))
	{
		//
		// smtp
		//

		case 60245459:
		{
			message->callback.push({this->name,"/send/smtp"});
		}
		break;

		//
		// sendgrid
		//

		case 3478885332:
		{
			message->data["task"]["data"]["template_id"] = it->second["id"].get<std::string>();
			message->callback.push({this->name,"/send/sendgrid"});
		}
		break;

		default:
		{
			std::cout << it->second << " " << tegia::crypt::crc32(it->second) << std::endl;
			exit(0);
		}
		break;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   
