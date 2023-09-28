#ifndef H_BASE_A2EXAMPLE
#define H_BASE_A2EXAMPLE

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
	#include <curl/curl.h>
	#include <chrono>

//	TEGIA 
	#include <tegia/tegia.h>
	#include <tegia/db/mysql/mysql.h>
	#include <tegia/actors/routers_map.h>
	#include <tegia/actors/actor_base.h>

	#include <tegia/core/http.h>

//	VENDORS


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTOR CLASS                                                                            //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define ACTOR_TYPE "BASE::EXAMPLE"



class message_iterate_t: public message_t
{	
	public:
		message_iterate_t(nlohmann::json json_data): message_t(json_data)
		{
			
		};

		~message_iterate_t()
		{
			
		};

		long long int count = 0;
};



class A2Example: public tegia::actors::actor_base
{	
	public:
		  
		A2Example(const std::string &name, nlohmann::json &data); 
		~A2Example(); 

		// ----------------------------------------------------------------------------------   
		// ACTION FUNCTIONS
		// ----------------------------------------------------------------------------------   

		std::string run(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

		std::string iterator_1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string iterator_2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

		std::string action_1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string action_2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

		std::string commit(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> t_start{};
		std::mutex commit_lock;
		long long int iterator = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


extern "C" tegia::actors::router_base * _load_actor()
{
	auto ro = new tegia::actors::router<A2Example>(ACTOR_TYPE);

	ACTION_ROUTE2( "/run/{case}",						&A2Example::run);

	ACTION_ROUTE2( "/iterator/1",						&A2Example::iterator_1);
	ACTION_ROUTE2( "/iterator/2",						&A2Example::iterator_2);

	ACTION_ROUTE2( "/action/1",							&A2Example::action_1);
	ACTION_ROUTE2( "/action/2",							&A2Example::action_2);

	ACTION_ROUTE2( "/commit",							&A2Example::commit);

	return ro;

	// {name}/{action}
	// http /api/v3/{name}/-/{action}
};

#endif

