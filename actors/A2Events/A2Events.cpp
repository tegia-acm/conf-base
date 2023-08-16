////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONSTRUCTOR / DESTRUCTOR                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "A2Events.h"

A2Events::A2Events(
	const std::string &name, 
	nlohmann::json &data):tegia::actors::actor_base(ACTOR_TYPE, name, data)
{ 

	this->router = new tegia::events::router();

//	TODO: Обработать ситуацию с ошибкой чтения из БД
	auto res = tegia::mysql::query("core","SELECT * FROM subscribers");
	for(int i = 0; i < res->count(); i++)
	{
		this->router->add_subscriber(
		res->get("subscribers::emiter_type",i),
		res->get("subscribers::emiter_id",i),
		res->get("subscribers::event_type",i),
		res->get("subscribers::actor",i),
		res->get("subscribers::action",i));
	}
	delete res;
}; 


A2Events::~A2Events()
{ 
	delete this->router;
};


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "actions/emit.cpp"
#include "actions/test.cpp"
#include "actions/test2.cpp"

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SUPPORT FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

