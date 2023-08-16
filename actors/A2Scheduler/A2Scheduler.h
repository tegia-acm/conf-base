#ifndef H_A2SCHEDULER
#define H_A2SCHEDULER

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	STL
	#include <regex>
	#include <string>
	#include <locale>
	#include <thread>
	#include <memory>

//	TEGIA 
	#include <tegia/tegia.h>
	#include <tegia/db/mysql/mysql.h>
	#include <tegia/actors/routers_map.h>
	#include <tegia/actors/actor_base.h>


//	VENDORS
	#include "../../vendors/ccronexpr/ccronexpr.h"

//	DAO
	#include "dao/dao_job.h"


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// Job CLASS                                                                              //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

class Job
{
	public:

		int id;
		std::string time;
		std::string actor;
		std::string action;
		nlohmann::json message;

		Job(const int id, const std::string &time, const std::string &actor, const std::string &action, 
			 nlohmann::json message): id(id), time(time), actor(actor), action(action), message(message) {};
};

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// A2Scheduler CLASS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define ACTOR_TYPE "A2Scheduler"

class A2Scheduler: public tegia::actors::actor_base
{
	public:

		// ----------------------------------------------------------------------------------   
		// ACTION FUNCTIONS
		// ----------------------------------------------------------------------------------   

		A2Scheduler(const std::string &name, nlohmann::json &init_data); 
		~A2Scheduler();  

		std::string run(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string jobs_get(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string job_add(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string job_save(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string job_delete(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);
		std::string job_meta(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

		std::string test1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);

	private:

		// ----------------------------------------------------------------------------------   
		// SUPPORT FUNCTIONS 
		// ----------------------------------------------------------------------------------   

		std::multimap<time_t,Job*> jobs;
		std::mutex mu;

		static time_t parse(const std::string &time);
		int load_job(
			int id, 
			const std::string &time, 
			const std::string &actor, 
			const std::string &action, 
			nlohmann::json message); 
};

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


extern "C" tegia::actors::router_base * _load_actor()
{
	auto ro = new tegia::actors::router<A2Scheduler>(ACTOR_TYPE);

	ACTION_ROUTE2( "/run",                           &A2Scheduler::run); 
	ACTION_ROUTE2( "/case/{gid}/jobs",               &A2Scheduler::jobs_get); 
	ACTION_ROUTE2( "/case/{gid}/jobs/add",           &A2Scheduler::job_add);   
	ACTION_ROUTE2( "/case/{gid}/jobs/{id}/save",     &A2Scheduler::job_save);   
	ACTION_ROUTE2( "/case/{gid}/jobs/{id}/delete",   &A2Scheduler::job_delete);   
	ACTION_ROUTE2( "/case/{gid}/jobs/{id}/meta",     &A2Scheduler::job_meta);   

	ACTION_ROUTE2( "/test1",                         &A2Scheduler::test1);   

	return ro;
};

#endif
