
////////////////////////////////////////////////////////////////////////////////////////////
/**
 * 
	Кто-то "излучил" событие, которое теперь нужно обработать

	POST /emit/{emiter_type}/{emiter_id}/{event_type}

	route_params["emiter_type"]
	route_params["emiter_id"]
	route_params["event_type"]

	message->root["event"]["t_create"] - дата события
	message->root["event"]["data"] - данные события

*/
////////////////////////////////////////////////////////////////////////////////////////////


std::string A2Events::emit(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responcedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	// ЗАПИСАТЬ СОБЫТИЕ В ЖУРНАЛ
		auto event = new DAO::EVENT_LOG();
			event->emiter_type(route_params["emiter_type"].get<std::string>());
			event->emiter_id(route_params["emiter_id"].get<std::string>());
			event->event_type(route_params["event_type"].get<std::string>());
			event->t_create(core::time::current());
			event->data(core::cast<std::string>(message->data["event"]["data"]));
			tegia::mysql::create("core",event);
		delete event;

	// НАЙТИ ОБРАБОТЧИКИ СОБЫТИЯ

	if(message->data["event"]["data"].is_null() == true)
	{
		// Неведомая фигня
		std::cout << "Неведомая фигня" << std::endl;
		//return "error";
	}

	nlohmann::json root;

	nlohmann::json subscribers = this->router->find(
	route_params["emiter_type"].get<std::string>(),
	route_params["emiter_id"].get<std::string>(),
	route_params["event_type"].get<std::string>());
  
	for(auto it = subscribers.begin(); it != subscribers.end(); it++)
	{
		root["event"]["data"] = (*it);

		// TODO: !!!
		std::string action = (*it)["action"].get<std::string>();
		std::size_t found;
		found = action.find("{emiter_id}");
		if (found!=std::string::npos)
		{
			action = action.replace(found,11,route_params["emiter_id"].get<std::string>());
		} 
		std::cout<<action<<std::endl;
		// !!!
		 
		tegia::message::send((*it)["actor"].get<std::string>(), action, root);
	}

	(*responcedata)["data"] = subscribers;

	/////////////////////////////////////////////////////////////////////////////////////
	return "200";
};


