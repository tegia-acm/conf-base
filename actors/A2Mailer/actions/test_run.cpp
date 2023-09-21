/////////////////////////////////////////////////////////////////////////////////////////
/*

	Action для запуска теста

*/
/////////////////////////////////////////////////////////////////////////////////////////


std::string A2Mailer::test_run(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	int id_test = core::cast<int>(route_params["id_test"].get<std::string>());
	nlohmann::json test_data = core::json::file(tegia::conf::path("base") + "/data/tests/test_" + core::cast<std::string>(id_test) + ".json");

	std::cout << "RUN TEST " << id_test << std::endl;

	auto msg = tegia::message::init();
	msg->data = test_data["run"];
	msg->callback.push({this->name,"/test/" + core::cast<std::string>(id_test) + "/check"});
	tegia::message::send(this->name,"/send",msg);

	return "ok";
}

