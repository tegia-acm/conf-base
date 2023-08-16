////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONSTRUCTOR / DESTRUCTOR                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "A2Scheduler.h"

A2Scheduler::A2Scheduler(
	const std::string &name, 
	nlohmann::json &init_data): tegia::actors::actor_base(ACTOR_TYPE, name, init_data)
{  }; 

A2Scheduler::~A2Scheduler()
{  };

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "actions/run.cpp"
#include "actions/jobs_get.cpp"
#include "actions/job_add.cpp"
#include "actions/job_save.cpp"
#include "actions/job_delete.cpp"
#include "actions/job_meta.cpp"

#include "actions/test1.cpp"

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SUPPORT FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

time_t A2Scheduler::parse(const std::string &time)
{
	time_t cur = std::time(NULL);
	time_t next;

	const char* err = NULL;
	cron_expr expr;
	memset(&expr, 0, sizeof(expr));

	cron_parse_expr(time.c_str(), &expr, &err);

	if(err)
	{
		std::cout << err << std::endl;
		std::string serr(err);
		throw serr;
	}

	next = cron_next(&expr, cur);
	delete[] err;

	return next;
};


int A2Scheduler::load_job(int id, const std::string &time, const std::string &actor, 
								  const std::string &action, nlohmann::json message)
{
	this->mu.lock();

	try
	{
		this->jobs.insert(std::make_pair( A2Scheduler::parse(time), new Job(id, time, actor, action, message) ));
	} 

	catch(std::string err)
	{
		std::cout << err << std::endl;
	}

	this->mu.unlock();
	return 0;
};


