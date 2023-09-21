/////////////////////////////////////////////////////////////////////////////////////////
/*

	"mail":
	{
		"from":
		{
			"email": <string>
		},
		"personalizations"
		[
			{
				"to":
				[
					{
						"email": <string>
					}
				],
				"dynamic_template_data":
				{
					// data
				}
			}
		]
	}

*/
/////////////////////////////////////////////////////////////////////////////////////////

std::string A2Mailer::sendgrid(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responcedata = &(message->data["responce"]["data"]);
	int _STATUS_ = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	std::string _from = message->data["task"]["data"]["from"].get<std::string>();

	//
	// Форматируем данные под sendgrid
	//

	std::string email = message->data["task"]["data"]["to"].get<std::string>();

	nlohmann::json _data = nlohmann::json::parse(R"(
	{
		"template_id": ")" + message->data["task"]["data"]["template_id"].get<std::string>() + R"(",
		"from":
		{
			"email": ")" + _from + R"("
		},
		"personalizations":
		[
			{
				"to":
				[
					{
						"email": ")" + email + R"("
					}
				],
				"dynamic_template_data":{}
			}
		]
	})");

	_data["personalizations"][0]["dynamic_template_data"] = message->data["task"]["data"]["personalizations"];

	//
	// Отправляем письмо
	//
	
	auto http = new tegia::http::client();
	http->set_header("Authorization","Bearer " + this->emails[_from]->sendgrid.api_key);
	http->set_header("Content-Type","application/json ");

	int res = http->post("https://api.sendgrid.com/v3/mail/send",_data.dump());

	switch(res)
	{
		case 202:
		{
			message->data["task"]["status"] = 200;

			LNOTICE("success send message to '" << email << "'");
		}
		break;

		default:
		{
			message->data["task"]["status"] = 500;
			message->data["task"]["error"]["code"] = res;
			message->data["task"]["error"]["info"] = http->response->data;

			LERROR("error send message to '" << email << "' code = " << res << "\n" << http->response->data)

			// std::cout << "status = " << res << std::endl;
			// std::cout << http->response->data << std::endl;
			// exit(0);

			//
			//  TODO:
			//    Записывать в БД информацию об отправке письма. Если при отправке письма произошла 
			//    ошибка (какая?), то повторить отправку письма через некоторое время. (Для этого нужно
			//    создать механизм "одноразовых" заданий)
			// 

		}
		break;
	}

	delete http;
	
	/////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};   
