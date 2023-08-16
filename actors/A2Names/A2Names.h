#ifndef _H_BASE_A2NAMES_
#define _H_BASE_A2NAMES_

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

//	TEGIA PLATFORM 
	#include <tegia/const.h>
	#include <tegia/core/cast.h>

	#include <tegia/tegia.h>
	#include <tegia/db/mysql/mysql.h>
	#include <tegia/actors/routers_map.h>
	#include <tegia/actors/actor_base.h>


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTOR CLASS                                                                            //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define ACTOR_TYPE "A2Names"

class A2Names: public tegia::actors::actor_base
{	
	public:

		// ----------------------------------------------------------------------------------   
		// ACTION FUNCTIONS
		// ----------------------------------------------------------------------------------   

		A2Names(const std::string &name, nlohmann::json &data); 
		~A2Names(); 

		std::string transform(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string get_sex(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string test(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string test1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

	private:

		// ----------------------------------------------------------------------------------   
		// SUPPORT FUNCTIONS 
		// ----------------------------------------------------------------------------------   

		std::unordered_map<std::string,int> sex_map;
		nlohmann::json unknown_names;
		bool load_names(void);


};

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


extern "C" tegia::actors::router_base * _load_actor()
{
	auto ro = new tegia::actors::router<A2Names>(ACTOR_TYPE);

	ACTION_ROUTE2( "/transform/{file}",					&A2Names::transform);
	ACTION_ROUTE2( "/get/{name}/sex",					&A2Names::get_sex);   
	ACTION_ROUTE2( "/test",								&A2Names::test);   
	ACTION_ROUTE2( "/test1",							&A2Names::test1);   

	return ro;
};



#endif
