/////////////////////////////////////////////////////////////////////////////////////////
/*



*/
/////////////////////////////////////////////////////////////////////////////////////////

std::string A2Example::action_1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responsedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	std::cout << _YELLOW_ << "ACTION 1" << _BASE_TEXT_ << std::endl; 
	std::this_thread::sleep_for(100ms);
	
	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   




std::string A2Example::action_2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responsedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	std::cout << _YELLOW_ << "ACTION 2" << _BASE_TEXT_ << std::endl; 
	std::this_thread::sleep_for(1000ms);
	
	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   