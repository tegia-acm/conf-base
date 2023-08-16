#ifndef H_DB_DAO_EVENT_LOG
#define H_DB_DAO_EVENT_LOG

#include <tegia/dao/entity.h>

////////////////////////////////////////////////////////////////////////////////////////////
/** 
	\brief
	\authors	Горячев Игорь
*/
////////////////////////////////////////////////////////////////////////////////////////////

namespace DAO { 

class EVENT_LOG: public dao::Entity
{
	protected:

		PROPDEFF2(std::string, emiter_type, "", nullptr, nullptr)
		PROPDEFF2(std::string, emiter_id, "", nullptr, nullptr)
		PROPDEFF2(std::string, event_type, "", nullptr, nullptr)
		PROPDEFF2(std::string, t_create, "", nullptr, nullptr)
		PROPDEFF2(std::string, data, "", nullptr, nullptr)

	public:   

		
		EVENT_LOG() : dao::Entity("events_log")
		{
			this->access->add("emiter_type", &DAO::EVENT_LOG::emiter_type, this);
			this->access->add("emiter_id", &DAO::EVENT_LOG::emiter_id, this);
			this->access->add("event_type", &DAO::EVENT_LOG::event_type, this);
			this->access->add("t_create", &DAO::EVENT_LOG::t_create, this);
			this->access->add("data", &DAO::EVENT_LOG::data, this);
		};

		virtual ~EVENT_LOG() {    };


};	// end EVENT_LOG class
}	// end namespace DAO








#endif 

