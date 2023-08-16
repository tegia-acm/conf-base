////////////////////////////////////////////////////////////////////////////////////////////
/** 

	ROUTE: /case/{gid}/jobs

	gid: <int>   case id

*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string A2Scheduler::jobs_get(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responcedata = &(message->data["responce"]["data"]);
	int _STATUS_ = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	std::string query = "SELECT * FROM _jobs";
	auto jobs = tegia::mysql::query("core",query); 
	(*responcedata)["status"] = "ok" ;
	(*responcedata)["data"] = jobs->json(); 

	// TODO: Гребаный костыль!!! Необходимо доработать DAO
	for(auto it = (*responcedata)["data"].begin(); it != (*responcedata)["data"].end(); it++ )
	{
		(*it)["message"] = nlohmann::json::parse((*it)["message"].get<std::string>());
	}

	delete jobs;

	_STATUS_ = 200;
	
	/////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};   

