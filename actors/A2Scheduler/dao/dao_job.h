#ifndef H_DB_DAO_JOB
#define H_DB_DAO_JOB

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

// STL 
	#include <string>
	#include <iostream>
	#include <sstream>

	#include <tegia/dao/entity.h>


namespace DAO { 

class JOB: public dao::Entity
{     
	protected:
		PROPDEFF2(int, _id_case, 0, nullptr, nullptr)
		PROPDEFF2(std::string, config, "", nullptr, nullptr)
		PROPDEFF2(std::string, time, "", nullptr, nullptr)
		PROPDEFF2(std::string, actor, "", nullptr, nullptr)
		PROPDEFF2(std::string, action, "", nullptr, nullptr)
		PROPDEFF2(std::string, message, "", nullptr, nullptr)
		PROPDEFF2(int, status, 0, nullptr, nullptr)

	public:
		JOB() : dao::Entity("_jobs")
		{
			this->access->add("_id_case", &DAO::JOB::_id_case, this);
			this->access->add("config", &DAO::JOB::config, this);
			this->access->add("time", &DAO::JOB::time, this);
			this->access->add("actor", &DAO::JOB::actor, this);
			this->access->add("action", &DAO::JOB::action, this);
			this->access->add("message", &DAO::JOB::message, this);
			this->access->add("status", &DAO::JOB::status, this);
		};

		virtual ~JOB() {  };


}; // END DAO::JOB CLASS


}  // END DAO NAMESPACE








#endif 

