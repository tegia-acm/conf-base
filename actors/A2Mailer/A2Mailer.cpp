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
	// Валидатор задач
	//

	this->email_task_validator.set_root_schema(
		core::json::file(tegia::conf::path("base") + "/data/schemas/task.json")
	);

	//
	// Валидатор конфиги
	//
	this->email_task_validator.set_root_schema(
		core::json::file(tegia::conf::path("base") + "/data/schemas/A2MailerConfig.json")
	);

	try
	{
		this->email_task_validator.validate(data);
	}
	catch(const std::exception& e)
	{
		LERROR("A2Mailer, actor '" << name << "': " << e.what())
		throw;
	}
	
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

#include "actions/test_run.cpp"
#include "actions/test_check.cpp"


std::string A2Mailer::get_templates(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	int res = 0;
	// int res = get_template(this->api_key, &this->templates);
	return core::cast<std::string>(res);
};


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SUPPORT FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


