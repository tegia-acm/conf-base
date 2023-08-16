#ifndef H_A2EVENTS
#define H_A2EVENTS

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	C++ STL
	#include <regex>
	#include <string>
	#include <locale>

//	TEGIA 
	#include <tegia/tegia.h>
	#include <tegia/db/mysql/mysql.h>
	#include <tegia/actors/routers_map.h>
	#include <tegia/actors/actor_base.h>

	#include <tegia/core/time.h>

//	DAO
	#include "dao/dao_events_log.h"

//	OTHER
	#include "other/events_router.h"


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTOR CLASS                                                                            //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define ACTOR_TYPE "A2Events"

class A2Events: public tegia::actors::actor_base
{
	protected:
		
		tegia::events::router * router;

		// ---------------------------------------------------------------------------------
		// SUPPORT FUNCTIONS 
		// ---------------------------------------------------------------------------------

	public:

		A2Events(const std::string &name, nlohmann::json &data);
		~A2Events();

		// ---------------------------------------------------------------------------------
		// ACTION FUNCTIONS
		// ---------------------------------------------------------------------------------

		std::string emit(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string test(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string test2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

};

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


extern "C" tegia::actors::router_base * _load_actor()
{
	auto ro = new tegia::actors::router<A2Events>(ACTOR_TYPE);

	ACTION_ROUTE2( "/emit/{emiter_type}/{emiter_id}/{event_type}/add",	&A2Events::emit);
	ACTION_ROUTE2( "/12/test",											&A2Events::test);
	ACTION_ROUTE2( "/34/test",											&A2Events::test2); 

	return ro;
};


#endif
