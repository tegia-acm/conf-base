////////////////////////////////////////////////////////////////////////////////////////////
/** 
	ROUTE: /index/{index_name}/update

*/
////////////////////////////////////////////////////////////////////////////////////////////

std::string A2Sphinx::update(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
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
	std::string sphinx_path = (*node_conf)["/sphinx/path"_json_pointer]->get<std::string>();
	*/

	std::string index_name = route_params["index_name"].get<std::string>();
	std::string command = this->paths.bin + "/indexer " + index_name + " --config " + this->paths.config + "/_all_index.conf --rotate";
	
	int ret = system( command.c_str() );

	////////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};