#ifndef _H_BASE_A2FILES_
#define _H_BASE_A2FILES_

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	C++ STL
	#include <regex>
	#include <string>
	#include <locale>
	#include <filesystem>
	#include <fstream>

//	TEGIA 
	#include <tegia/tegia.h>
	#include <tegia/db/mysql/mysql.h>
	#include <tegia/actors/routers_map.h>
	#include <tegia/actors/actor_base.h>
	#include <tegia/core/http2.h>
	#include <tegia/core/http.h>
	#include <tegia/core/string.h>


//	VENDORS
 	#include <xml2json/include/xml2json.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTOR CLASS                                                                            //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define ACTOR_TYPE "A2Files"

class A2Files: public tegia::actors::actor_base
{	
	public:

		// ----------------------------------------------------------------------------------   
		// ACTION FUNCTIONS
		// ----------------------------------------------------------------------------------   

		A2Files(const std::string &name, nlohmann::json &data); 
		~A2Files(); 

		std::string http_load(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string http_load2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string unzip(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params) const;
		std::string xml_to_json(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string iconv(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

		std::string http_test(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

	protected:

		// ----------------------------------------------------------------------------------   
		// SUPPORT FUNCTIONS 
		// ----------------------------------------------------------------------------------   

		nlohmann::json_schema::json_validator http_load_validator;
};

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

extern "C" tegia::actors::router_base * _load_actor()
{
	auto ro = new tegia::actors::router<A2Files>(ACTOR_TYPE);

	ACTION_ROUTE2( "/http/load",					&A2Files::http_load);
	ACTION_ROUTE2( "/http/load2",					&A2Files::http_load2);
	ACTION_ROUTE2( "/unzip",						&A2Files::unzip);
	ACTION_ROUTE2( "/iconv",						&A2Files::iconv);
	ACTION_ROUTE2( "/xml2json",						&A2Files::xml_to_json);   

	ACTION_ROUTE2( "/http/test",					&A2Files::http_test);


	return ro;
};

#endif
