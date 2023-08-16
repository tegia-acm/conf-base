////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONSTRUCTOR / DESTRUCTOR                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "A2Mailer.h"

////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json get_request_param(const std::string & method, const std::string & token)
{
	nlohmann::json request_param;

	request_param["method"] = method;
	request_param["useragent"] = "tegia server";
	request_param["headers"].push_back("Authorization: Bearer " + token);
	request_param["headers"].push_back("Content-Type: application/json");

	return request_param;
};

////////////////////////////////////////////////////////////////////////////////////////////

/*int get_template(const std::string &api_key, std::unordered_map <std::string,std::string> *templates)
{
	//
	// Получаем список шаблонов
	//

	// auto http = new core::http2();

	auto http = new tegia::http::client();
	http->set_header("Authorization","Bearer " + api_key);
	http->set_header("Content-Type","application/json ");

	//nlohmann::json request_param = get_request_param("GET", api_key);
	int res = http->get("https://api.sendgrid.com/v3/templates?generations=dynamic");
	switch(res)
	{
		case 200:
		{
			nlohmann::json data = nlohmann::json::parse(http->response->data)["templates"];
			for(auto it = data.begin(); it != data.end(); it++)
			{
				std::cout << (*it)["name"].get<std::string>() << " :: " << (*it)["id"].get<std::string>() << std::endl; 
				templates->insert( { (*it)["name"].get<std::string>(), (*it)["id"].get<std::string>() } );
			}
		}
		break;

		default:
		{
			std::cout << "[" << res << "] https://api.sendgrid.com/v3/templates" << std::endl;
		}
		break;
	}

	delete http;
	return res;
};*/

////////////////////////////////////////////////////////////////////////////////////////////

A2Mailer::A2Mailer(
	const std::string &name, 
	nlohmann::json &data)
	: tegia::actors::actor_base(ACTOR_TYPE, name ,data),
	  is_active(false),
	  api_key("")
{ 

	//
	// Валидатор
	//

	this->send_email_validator.set_root_schema(R"json(
	{
		"$schema": "http://json-schema.org/draft-07/schema#",
		"title": "EMAIL_SEND TASK schema",
		"type": "object",
		"properties": 
		{
			"task":
			{
				"type": "object",
				"properties":
				{
					"uuid":
					{
						"description": "Уникальный идентификатор задачи",
						"type": "string",
						"pattern": "[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}"
					},
					"type":
					{
						"description": "Тип задачи",
						"type": "string",						
						"pattern": "EMAIL_SEND"
					},
					"data":
					{
						"type": "object",
						"properties":
						{
							"from":
							{
								"description": "email, от которого отправляется письмо",
								"type": "string"								
							},
							"to":
							{
								"description": "email кому направляется письмо",
								"type": "string"								
							},
							"subject":
							{
								"description": "Заголовок письма",
								"type": "string"
							},
							"template":
							{
								"description": "Имя шаблона, по которому необходимо сформировать письмо",
								"type": "string"
							}
						},
						"required": ["from", "to", "subject", "template"]
					}
				},
				"required": ["uuid", "type", "data"]
			}
		},
		"required": ["task"]
	})json"_json);

	//
	// Читаем шаблоны
	//

	for(auto it = data["templates"].begin(); it != data["templates"].end(); it++)
	{
		this->templates.insert({(*it)["name"].get<std::string>(),(*it)});
	}

	//
	// Читаем настройки для emails
	//

	for(auto it = data["emails"].begin(); it != data["emails"].end(); it++)
	{
		this->emails.insert({(*it)["email"].get<std::string>(),std::make_shared<BASE::MAIL>(*it) });
	}

	std::cout << "email size = " << this->emails.size() << std::endl;

}; 

A2Mailer::~A2Mailer() { };

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


#include "actions/send.cpp"

#include "actions/smtp.cpp"
#include "actions/sendgrid.cpp"


std::string A2Mailer::get_templates(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	int res = 0;
	// int res = get_template(this->api_key, &this->templates);
	return core::cast<std::string>(res);
};





std::string A2Mailer::test_send(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	int id_test = core::cast<int>(route_params["id_test"].get<std::string>());
	switch(id_test)
	{
		case 1:
		{
			nlohmann::json personalization;
			personalization["to"].push_back("{\"email\":\"goryachev-igor@yandex.ru\"}"_json);
			personalization["dynamic_template_data"]["full_name"] = "Горячев Игорь";
			personalization["dynamic_template_data"]["confirm_url"] = "http://dit.local/confirm/sfds-sdfds-sdfdsf-sdfdsf";
			personalization["dynamic_template_data"]["test"] = "fuck the system";
			personalization["dynamic_template_data"]["news"].push_back("{\"text\":\"test news 01\"}"_json);
			personalization["dynamic_template_data"]["news"].push_back("{\"text\":\"test news 02\"}"_json);
			
			nlohmann::json mail;
			mail["mail"]["from"]["email"] = "igor@tegia.ru";
			mail["mail"]["personalizations"].push_back(personalization);

			tegia::message::send(this->name,"/send/email_confirm_template",mail);
		}
		break;

		case 2:
		{
			nlohmann::json personalization;
			personalization["to"].push_back("{\"email\":\"goryachev-igor@yandex.ru\"}"_json);
			personalization["dynamic_template_data"]["company"] = "ООО \"Рога и копыта\"";
			personalization["dynamic_template_data"]["person"] = "Горячев Игорь";
			personalization["dynamic_template_data"]["email"] = "test@test.ru";
			personalization["dynamic_template_data"]["phone"] = "+79109201020";
			personalization["dynamic_template_data"]["comment"] = "Все хочу!!!";
			
			nlohmann::json mail;
			mail["mail"]["from"]["email"] = "igor@tegia.ru";
			mail["mail"]["personalizations"].push_back(personalization);

			tegia::message::send(this->name,"/send/new_landing_order",mail);
		}
		break;
	}

	return "ok";
}

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SUPPORT FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


