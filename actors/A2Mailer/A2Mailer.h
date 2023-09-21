#ifndef _H_BASE_A2MAILER_
#define _H_BASE_A2MAILER_

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	C++ STL
	#include <regex>
	#include <locale>
	#include <filesystem>

	#include <queue>
	#include <unordered_set>
	#include <curl/curl.h>

//	TEGIA 
	#include <tegia/tegia.h>
	#include <tegia/db/mysql/mysql.h>
	#include <tegia/actors/routers_map.h>
	#include <tegia/actors/actor_base.h>

	#include <tegia/core/http.h>

// VENDORS

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTOR CLASS                                                                            //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define ACTOR_TYPE "BASE::MAILER"

namespace BASE {

struct SMTP
{
	std::string server;
	std::string email;
	std::string password;
};

struct SENDGRID
{
	std::string email;
	std::string api_key;
};

struct MAIL
{
	std::string email;
	BASE::SMTP smtp;
	BASE::SENDGRID sendgrid;

	MAIL(const nlohmann::json &_data)
	{
		//std::cout << _data.dump() << std::endl;

		this->email = _data["email"].get<std::string>();
		
		if(_data.contains("smtp") == true)
		{
			this->smtp.server = _data["smtp"]["server"].get<std::string>();
			this->smtp.password = _data["smtp"]["password"].get<std::string>();
			this->smtp.email = this->email;

			//std::cout << this->smtp.server << std::endl;
		}

		if(_data.contains("app.sendgrid.com") == true)
		{
			this->sendgrid.api_key = _data["app.sendgrid.com"]["api_key"].get<std::string>();
			this->sendgrid.email = this->email;
		}
	};
};

const std::unordered_set<std::string> supported_senders
{
	"smtp",
	"sendgrid"
};

}	// END namespace BASE


class A2Mailer: public tegia::actors::actor_base
{	
	public:
		  
		A2Mailer(const std::string &name, nlohmann::json &data); 
		~A2Mailer(); 

		// ----------------------------------------------------------------------------------   
		// ACTION FUNCTIONS
		// ----------------------------------------------------------------------------------   

		std::string send(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string get_templates(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

		std::string smtp(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string sendgrid(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

		std::string test_run(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string test_check(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);


	private:

		std::string api_key;
		bool is_active;

		std::unordered_map <std::string,nlohmann::json> templates;
		std::unordered_map <std::string,std::shared_ptr<BASE::MAIL>> emails;


		// ----------------------------------------------------------------------------------   
		// SUPPORT FUNCTIONS 
		// ----------------------------------------------------------------------------------   

		nlohmann::json_schema::json_validator email_task_validator;

		bool validate_template(const nlohmann::json &data);
};

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


extern "C" tegia::actors::router_base * _load_actor()
{
	auto ro = new tegia::actors::router<A2Mailer>(ACTOR_TYPE);

	ACTION_ROUTE2( "/send",								&A2Mailer::send);
	ACTION_ROUTE2( "/templates",						&A2Mailer::get_templates);

	ACTION_ROUTE2( "/send/smtp",						&A2Mailer::smtp);
	ACTION_ROUTE2( "/send/sendgrid",					&A2Mailer::sendgrid);

	ACTION_ROUTE2( "/test/{id_test}/run",				&A2Mailer::test_run);
	ACTION_ROUTE2( "/test/{id_test}/check",				&A2Mailer::test_check);

	return ro;
};

#endif // END _H_BASE_A2MAILER_

