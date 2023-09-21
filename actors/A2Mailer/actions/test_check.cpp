


std::string A2Mailer::test_check(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{

	int id_test = core::cast<int>(route_params["id_test"].get<std::string>());
	nlohmann::json test_data = core::json::file(tegia::conf::path("base") + "/data/tests/test_" + core::cast<std::string>(id_test) + ".json");

	std::string path = test_data["check"]["path"].get<std::string>(); 
	int test_status = test_data["check"]["value"].get<int>();

	nlohmann::json::json_pointer ptr(path);
	int curr_status = message->data[ptr].get<int>();

	if(test_status == curr_status)
	{
		std::cout << _OK_TEXT_ << "TEST " << id_test << std::endl;
	}
	else
	{
		std::cout << _ERR_TEXT_ << "TEST " << id_test << std::endl;
		std::cout << message->data << std::endl;
	}

	return "ok";
}
