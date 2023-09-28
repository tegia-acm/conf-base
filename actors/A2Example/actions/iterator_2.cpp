/////////////////////////////////////////////////////////////////////////////////////////
/*



*/
/////////////////////////////////////////////////////////////////////////////////////////


void _for(const std::shared_ptr<message_t> &message, int count, 
	std::function<void(const std::shared_ptr<message_iterate_t> &message)> _fn_loop,
	std::function<void(const std::shared_ptr<message_iterate_t> &message)> _fn_init)
{
	auto msg = std::dynamic_pointer_cast<message_iterate_t>(message);
	if(msg)
	{
		if(msg->count < count)
		{
			msg->count++;
			_fn_loop(msg);
		}
		else
		{
			std::cout << "END ITERATE" << std::endl;
		}
	}
	else
	{
		msg = tegia::message::init<message_iterate_t>();
		msg->count = 0;
		_fn_init(msg);
	}
};


std::string A2Example::iterator_2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responsedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	_for(message,100,
		[this](const std::shared_ptr<message_iterate_t> &_message)
		{
			std::cout << "[" << _message->count << "]" << std::endl;
		
			tegia::message::send("base/example","/action/1",tegia::message::init());
			tegia::message::send("base/example","/iterator/2",_message);
		},
		[this](const std::shared_ptr<message_iterate_t> &_message)
		{
			this->iterator = 100;
			tegia::message::send("base/example","/iterator/2",_message);
		}
	);
	
	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};