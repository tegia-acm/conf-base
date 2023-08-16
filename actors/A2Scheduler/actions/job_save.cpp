////////////////////////////////////////////////////////////////////////////////////////////
/** 

	ROUTE: /case/{gid}/jobs/{id}/save

	gid: <int>   case id
	id:  <int>   job id

	DATA:
	 
	message->root["request"]
	{
		_id: <int>,
		_id_case: <int>,
		config: <string>,
		time: <string>,
		actor: <string>,
		action: <string>,
		message: <JSON>,
		status: <int>,
	}

	Права доступа

	Вносить изменения в задачу могут:
		- участники кейса, к которому принадлежит задача;
		- участники кейсов с классом scheduler

*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string A2Scheduler::job_save(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responcedata = &(message->data["responce"]["data"]);
	int _STATUS_ = 0;
	//////////////////////////////////////////////////////////////////////////////////////////

	int _id_case = core::cast<int>(route_params["gid"].get<std::string>());
	int _id_job = core::cast<int>(route_params["id"].get<std::string>());

	// ---------------------------------------------------------------------------------------
	// Записываем изменения в БД
	// ---------------------------------------------------------------------------------------

	auto res = tegia::mysql::read("core", new DAO::JOB, "_id = " + core::cast<std::string>(_id_job));
	int old_status = res.obj->status();

	if(message->data["request"]["config"].is_null() == false)
	{
		res.obj->config( message->data["request"]["config"].get<std::string>());
	}

	if(message->data["request"]["time"].is_null() == false)
	{
		res.obj->time( message->data["request"]["time"].get<std::string>());
	}

	if(message->data["request"]["actor"].is_null() == false)
	{
		res.obj->actor( message->data["request"]["actor"].get<std::string>());
	}

	if(message->data["request"]["action"].is_null() == false)
	{
		res.obj->action( message->data["request"]["action"].get<std::string>());
	}
	if(message->data["request"]["message"].is_null() == false)
	{
		LDEBUG("job save: " + message->data["request"]["message"].dump() );
		res.obj->message( message->data["request"]["message"] );
	}

	if(message->data["request"]["status"].is_null() == false)
	{
		res.obj->status( core::cast<int>(message->data["request"]["status"].get<std::string>()) );
	} 

	//
	// TODO: (!!!!!)
	//   Не определено поведение системы в случае, если данных по заданию нет
	//

	tegia::mysql::update("core", res.obj);
 
	// ---------------------------------------------------------------------------------------
	// Обновляем задачу в планировщике
	// ---------------------------------------------------------------------------------------
 
	this->mu.lock();

	// Добавить задачу
	if( (old_status != res.obj->status()) && (res.obj->status() == 1) )
	{
		this->jobs.insert(
			std::make_pair( A2Scheduler::parse(res.obj->time() ), 
			new Job(_id_job, res.obj->time(), res.obj->actor(), res.obj->action(), res.obj->message() ) )
		);
	}
	else
	{
		for(auto it = this->jobs.begin(); it != this->jobs.end(); it++)
		{
			if(it->second->id == _id_job)
			{
				auto job = it->second;
				job->time = res.obj->time();
				job->actor = res.obj->actor();
				job->action = res.obj->action();
				job->message = res.obj->message();
				this->jobs.erase(it);
				if(res.obj->status() == 1)
				{
					this->jobs.insert(std::make_pair( A2Scheduler::parse(job->time), job ));
				}
				break;
			}       
		}
	}
	
	this->mu.unlock();

	delete res.obj;

	(*responcedata)["status"] = "ok";
	_STATUS_ = 200;
	
	/////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};   


