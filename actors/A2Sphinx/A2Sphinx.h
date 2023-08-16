#ifndef H_A2SPHINX
#define H_A2SPHINX

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


//	TEGIA 
	#include <tegia/tegia.h>
	#include <tegia/db/mysql/mysql.h>
	#include <tegia/actors/routers_map.h>
	#include <tegia/actors/actor_base.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// A2Sphinx CLASS                                                                         //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define ACTOR_TYPE "A2Sphinx"



struct PATH
{
	std::string base;
	std::string data;
	std::string bin;
	std::string config;
};


class A2Sphinx: public tegia::actors::actor_base
{	
	public:

		A2Sphinx(const std::string &name, nlohmann::json &data); 
		~A2Sphinx();  

		// ----------------------------------------------------------------------------------   
		// ACTION FUNCTIONS
		// ----------------------------------------------------------------------------------   

		std::string create(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string update(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string restart(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

	protected:

		PATH paths;


		// ----------------------------------------------------------------------------------   
		// SUPPORT FUNCTIONS 
		// ----------------------------------------------------------------------------------   


};

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


extern "C" tegia::actors::router_base * _load_actor()
{
	auto ro = new tegia::actors::router<A2Sphinx>(ACTOR_TYPE);

	ACTION_ROUTE2("/index/{index_name}/create",			&A2Sphinx::create);
	ACTION_ROUTE2("/index/{index_name}/update",			&A2Sphinx::update);
	ACTION_ROUTE2("/restart",							&A2Sphinx::restart);

	return ro;
};

#endif