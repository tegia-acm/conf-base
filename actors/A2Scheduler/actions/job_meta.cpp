
////////////////////////////////////////////////////////////////////////////////////////////
/** 

	ROUTE: /case/{gid}/jobs/{id}/meta

	gid: <int>   case id
	id:  <int>   job id

*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string A2Scheduler::job_meta(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responcedata = &(message->data["responce"]["data"]);
	int _STATUS_ = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	int _id_job = core::cast<int>(route_params["id"].get<std::string>());
	auto job = tegia::mysql::read("core", new DAO::JOB(), "_id = " + core::cast<std::string>(_id_job));

	if( job.code == 200 )
	{
		(*responcedata)["status"] = "ok";
		(*responcedata)["data"]["meta"] = job.obj->message();
		(*responcedata)["data"]["time"] = job.obj->time();
		_STATUS_ = 200;
	} 
	else
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 500;
		(*responcedata)["error"]["message"] = "Ошибка чтения БД";
		_STATUS_ = 500;
	}
	delete job.obj;

	/////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};


