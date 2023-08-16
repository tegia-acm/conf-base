////////////////////////////////////////////////////////////////////////////////////////////
/** 
	ROUTE: /restart

*/
////////////////////////////////////////////////////////////////////////////////////////////

std::string A2Sphinx::restart(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	int _STATUS_ = 200;
	////////////////////////////////////////////////////////////////////////////////////////

	//
	// Читаем конфигурационный файл
	//

	/*
	auto node_conf = tegia::conf::get("node");
	std::string base_path = tegia::conf::path("base") + "/actors/A2Sphinx";
	std::string config_path = base_path + "/configs";
	std::string sphinxdata_path = (*node_conf)["/sphinx/data"_json_pointer]->get<std::string>();
	std::string sphinx_path = (*node_conf)["/sphinx/path"_json_pointer]->get<std::string>();
	*/

	//
	// Генерирум конфиг для sphinx и запускаем его
	//

	int ret = system(("bash " +  this->paths.base + "/generate_conf.sh " + this->paths.config + " " + this->paths.data + " " + this->paths.base).c_str());

	////////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};