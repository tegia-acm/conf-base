////////////////////////////////////////////////////////////////////////////////////////////
/** 

*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string A2Events::test2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responcedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	std::cout << "======================================================" << std::endl;
	std::cout << "test 34" << std::endl;
	std::cout << "======================================================" << std::endl;

	/////////////////////////////////////////////////////////////////////////////////////
	return "200";
};

