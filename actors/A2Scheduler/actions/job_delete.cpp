////////////////////////////////////////////////////////////////////////////////////////////
/** 

	ROUTE: /case/{gid}/jobs/{id}/delete

	gid: <int>   case id
	id:  <int>   job id

*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string A2Scheduler::job_delete(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responcedata = &(message->data["responce"]["data"]);
	int _STATUS_ = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	int _id_case = core::cast<int>(route_params["gid"].get<std::string>());
	int _id_job = core::cast<int>(route_params["id"].get<std::string>());

	// Удаляем задачу из БД
	auto job = new DAO::JOB();
	job->_id(_id_job);
	tegia::mysql::delet("core", job, "_id");
	delete job;

	// Удаляем задачу в планировщике
	for(auto it = this->jobs.begin(); it != this->jobs.end(); it++)
	{
		if(it->second->id == _id_job)
		{
			this->jobs.erase(it);
			break;
		}     
	}

	(*responcedata)["status"] = "ok";
	_STATUS_ = 200;

	/////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};   




