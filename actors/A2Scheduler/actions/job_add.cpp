
////////////////////////////////////////////////////////////////////////////////////////////
/** 

	--------------------------------------------
	ROUTE: /case/{gid}/jobs/add
	--------------------------------------------

	gid: <int>   case id

	--------------------------------------------
	INPUT MESSAGE:
	--------------------------------------------

	request
	{
		_id_case: <int>,
		config: <string>,
		time: <string>,
		actor: <string>,
		action: <string>,
		message: <JSON>,
		status: <int>,
	}

	--------------------------------------------
	OUTPUT MESSAGE:
	--------------------------------------------

	OK
		
	{
		"status": "ok",
		"data":
		{
			"_id": <int>
		}
	}

	ERROR
	
	{
		"status": "error",
		"error"
		{
			"code": <int>
		}
	}

*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string A2Scheduler::job_add(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responcedata = &(message->data["responce"]["data"]);
	int _STATUS_ = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	if( !route_params.contains( "gid" ) )
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 400;
		(*responcedata)["error"]["info"] = "Required parameter [gid] in the route params";

		return "400";
	}

	std::string str_config;
	std::string str_message = "{}";

	if( message->data["request"].contains( "config" ) )
	{
		if( message->data["request"]["config"].is_string() )
		{
			str_config = message->data["request"]["config"].get<std::string>();
		}
		else
		{
			(*responcedata)["status"] = "error";
			(*responcedata)["error"]["code"] = 400;
			(*responcedata)["error"]["info"] = "Parameter [config] must be a string";

			return "400";			
		}
	}	

	if( message->data["request"].contains( "message" ) )
	{
		if( message->data["request"]["message"].is_string() )
		{
			str_message = message->data["request"]["message"].get<std::string>();

			try
			{
				nlohmann::json( std::move( nlohmann::json::parse( str_message ) ) );
			}
			catch( ... )
			{
				(*responcedata)["status"] = "error";
				(*responcedata)["error"]["code"] = 400;
				(*responcedata)["error"]["info"] = "Parameter [message] must be a valid json object";

				return "400";
			}
			
		}
		else
		{
			(*responcedata)["status"] = "error";
			(*responcedata)["error"]["code"] = 400;
			(*responcedata)["error"]["info"] = "Parameter [message] must be a string";

			return "400";			
		}
	}	

	if( !message->data[ "request" ].contains( "time" ) )
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 400;
		(*responcedata)["error"]["info"] = "Required parameter [time]";

		return "400";
	}
	else if( !message->data["request"]["time"].is_string() )
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 400;
		(*responcedata)["error"]["info"] = "Parameter [time] must be a string";

		return "400";
	}
	
	if( !message->data[ "request" ].contains( "actor" ) )
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 400;
		(*responcedata)["error"]["info"] = "Required parameter [actor]";

		return "400";
	}
	else if( !message->data["request"]["actor"].is_string() )
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 400;
		(*responcedata)["error"]["info"] = "Parameter [actor] must be a string";

		return "400";
	}

	if( !message->data[ "request" ].contains( "action" ) )
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 404;
		(*responcedata)["error"]["info"] = "Required parameter [actor]";

		return "400";
	}
	else if( !message->data["request"]["action"].is_string() )
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 400;
		(*responcedata)["error"]["info"] = "Parameter [action] must be a string";

		return "400";
	}

	if( !message->data[ "request" ].contains( "status" ) )
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 400;
		(*responcedata)["error"]["info"] = "Required parameter [status]";

		return "400";
	}
	else if( !message->data["request"]["status"].is_number() )
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = 400;
		(*responcedata)["error"]["info"] = "Parameter [status] must be a number";

		return "400";
	}

	int id_case = core::cast<int>(route_params["gid"].get<std::string>());

	// Записываем информацию
	std::unique_ptr< DAO::JOB > job( new DAO::JOB() );

	job->_id_case( id_case );
	job->config( str_config );
	job->time(message->data["request"]["time"].get<std::string>());
	job->actor(message->data["request"]["actor"].get<std::string>());
	job->action(message->data["request"]["action"].get<std::string>());
	job->message( str_message );
	job->status( message->data["request"]["status"] );

	auto res = tegia::mysql::create("core", job.get() );
	if(res.code == 200)
	{
		(*responcedata)["status"] = "ok";
		(*responcedata)["data"]["_id"] = job->_id();

		try
		{
			this->load_job(
				job->_id(), 
				job->time(), 
				job->actor(), 
				job->action(), 
				message->data["request"]["message"] 
			);
		}
		catch(std::string err)
		{
			(*responcedata)["status"] = "error";
			(*responcedata)["error"]["code"] = 400;
			(*responcedata)["error"]["info"] = err;

			tegia::mysql::delet( "core", job.get(), "_id" );

			return "400";	
		}
		catch(const std::exception& e)
		{
			(*responcedata)["status"] = "error";
			(*responcedata)["error"]["code"] = 400;
			(*responcedata)["error"]["info"] = e.what();

			tegia::mysql::delet( "core", job.get(), "_id" );

			return "400";	
		}
		
		LDEBUG("job add: " + message->data["request"]["message"].dump());

		_STATUS_ = 200;
	}
	else
	{
		(*responcedata)["status"] = "error";
		(*responcedata)["error"]["code"] = res.code;

		LERROR("job add: " + message->data["request"]["message"].dump());

		_STATUS_ = 500;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};

