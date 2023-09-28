/////////////////////////////////////////////////////////////////////////////////////////
/*



*/
/////////////////////////////////////////////////////////////////////////////////////////

std::string A2Example::iterator_1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responsedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	this->count = 100;
	for(auto i = 0; i < this->count; ++i)
	{
		std::cout << "[" << i << "]" << std::endl;

		auto msg = tegia::message::init();
		msg->callback.push({this->name,"/action/2"});
		tegia::message::send("base/example","/action/1",msg);
	}
	
	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   






void _for(const std::shared_ptr<message_t> &message, int count, std::function<void(const std::shared_ptr<message_iterate> &message)> _fn)
{
	auto msg = std::dynamic_pointer_cast<message_iterate>(message);
	if(msg)
	{
		if(msg->count < count)
		{
			_fn(msg);
			msg->count++;
			tegia::message::send("base/example","/iterator/2",msg);			
		}
		else
		{
			std::cout << "END ITERATE" << std::endl;
		}
	}
	else
	{
		msg = tegia::message::init<message_iterate>();
		msg->count = 0;
		tegia::message::send("base/example","/iterator/2",msg);
	}
};


std::string A2Example::iterator_2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responsedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	/*
	auto msg = std::dynamic_pointer_cast<message_iterate>(message);
	if(msg)
	{
		if(msg->count < 100)
		{
			auto msg2 = tegia::message::init();
			msg2->callback.push({this->name,"/action/2"});			
			tegia::message::send("base/example","/action/1",msg2);

			msg->count++;
			tegia::message::send(this->name,"/iterator/2",msg);			
		}
		
	}
	else
	{
		msg = tegia::message::init<message_iterate>();
		msg->count = 0;
		tegia::message::send(this->name,"/iterator/2",msg);
	}*/

	_for(message,100,[this](const std::shared_ptr<message_iterate> &_message){

			std::cout << "[" << _message->count << "]" << std::endl;

			auto msg = tegia::message::init();
			msg->callback.push({this->name,"/action/2"});			
			tegia::message::send("base/example","/action/1",msg);
	});
	
	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   