/////////////////////////////////////////////////////////////////////////////////////////
/*



*/
/////////////////////////////////////////////////////////////////////////////////////////

std::string A2Example::run(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responsedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	this->t_start = std::chrono::high_resolution_clock::now();
	int _case = core::cast<int>(route_params["case"].get<std::string>());

	std::cout << "RUN TEST " << _case << std::endl;
	tegia::message::send("base/example","/iterator/" + core::cast<std::string>(_case),tegia::message::init());
	
	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   



