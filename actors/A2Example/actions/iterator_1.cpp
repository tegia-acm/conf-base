/////////////////////////////////////////////////////////////////////////////////////////
/*



*/
/////////////////////////////////////////////////////////////////////////////////////////

std::string A2Example::iterator_1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responsedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	this->iterator = 100;
	for(auto i = 0; i < this->iterator; ++i)
	{
		std::cout << "[" << i << "]" << std::endl;

		auto msg = tegia::message::init();
		msg->callback.push({this->name,"/commit"});
		msg->callback.push({this->name,"/action/2"});
		tegia::message::send("base/example","/action/1",msg);
	}
	
	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   

