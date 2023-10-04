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

	auto create_error_info = [&message](int status, const std::string& task_msg, const std::string& log_msg)
	{
		message->data["task"]["status"] = status;
		message->data["task"]["error"]["code"] = 1;
		message->data["task"]["error"]["info"] = task_msg;

		LERROR(log_msg << message->data.dump());

		return std::to_string(status);
	};

	//
	// Валидируем формат задачи
	//
	
	try
	{
		this->email_task_validator.validate(message->data);
	}

	catch(const std::exception& e)
	{
		return create_error_info(400, e.what(), "[400] DATA NOT VALID\n");
	}

	//
	// Проверяем, что для _from есть настройки в конфигурации 
	//

	std::string _from = message->data["task"]["data"]["from"].get<std::string>();
	auto pos = this->emails.find(_from);
	if(pos == this->emails.end())
	{
		const std::string msg = "email '" + _from + "' configs not found";
		return create_error_info(404, msg, "[404] EMAIL NOT FOUND\n");
	}
	
	//
	// По шаблону определяем способ отправки
	// 

	const std::string _template = message->data["task"]["data"]["template"].get<std::string>();
	auto it = this->templates.find(_template);
	if(it == this->templates.end())
	{
		const std::string msg = "template not found '" + _template + "'";
		return create_error_info(404, msg, "[404] TEMPLATE NOT FOUND\n");
	}

	auto const sender_type = it->second["type"].get<std::string>();
	switch(tegia::crypt::crc32(sender_type))
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
			const std::string msg = "unsupported sender type: " + sender_type;
			return create_error_info(406, msg, "[406] UNSUPPORTED SENDER TYPE\n");
		}
		break;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   
