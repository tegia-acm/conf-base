#include <tegia/core/time.h>

////////////////////////////////////////////////////////////////////////////////////////////
/** 

	ROUTE: /run

*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string A2Scheduler::run(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	int _STATUS_ = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	// Читаем какие конфигурации нужно загружать

	auto _config = tegia::conf::get("node");
	auto configurations = (*_config)["configurations"];

	std::string cn = "";
	for(auto it = configurations->begin(); it != configurations->end(); it++)
	{
		if((*it)["isload"].get<bool>() == true)
		{
			cn = cn + ",'" + (*it)["name"].get<std::string>() + "'";	
		}
	}

	/*auto conf = context::node()->config;

	std::string cn = "";
	for(auto it = conf["configurations"].begin(); it != conf["configurations"].end(); it++)
	{
		if( (*it)["isload"].get<bool>() == true)
		{
			cn = cn + ",'" + (*it)["name"].get<std::string>() + "'";
		}
	}*/

	cn = cn.substr(1);

	std::cout << "Scheduler RUN: " << cn << std::endl;

	std::string query = "SELECT * FROM _jobs WHERE config IN (" + cn + ") AND status = 1";
	auto jobs = tegia::mysql::query("core",query); 

	std::cout << "jobs found: " << jobs->count() << "" << std::endl;     

	for(int i = 0; i < jobs->count(); i++)
	{
		try
		{
			this->load_job( 
				core::cast<int>(jobs->get("_jobs::_id",i)), 
				jobs->get("_jobs::time",i), 
				jobs->get("_jobs::actor",i), 
				jobs->get("_jobs::action",i), 
				nlohmann::json::parse(jobs->get("_jobs::message",i)) 
			); 
		}
		catch( ... )
		{

		}
	}  

	delete jobs; 

	this->mu.lock();

	time_t curr = std::time(NULL);
	time_t first = ( this->jobs.size() > 0 )? this->jobs.begin()->first : curr + 2;

	int div = (first - curr) % 60;

	std::this_thread::sleep_for(std::chrono::seconds(div));
	curr = std::time(NULL);

	int step = 1;

	while(true)
	{
		std::string logs;
		logs = "--- START LOOP: " + std::to_string(step) + "\n";
		logs = core::string::format() << logs << "        curr time " << core::time::unixtime_to_str( curr ) << "\n";
		
		for(auto it = this->jobs.begin(); it != this->jobs.end(); it++)
		{
			logs = core::string::format() << logs << "        " << core::time::unixtime_to_str( it->first ) << " " << it->second->time 
					<< " " << it->second->actor << " " << it->second->action << "\n";
		}

		// Берем первую задачу из пула задач
		auto it = this->jobs.begin();
		if( it != this->jobs.end() && (it->first - curr) < 1 )
		{
			Job *job = it->second;
 			
 			logs = core::string::format() << logs << "        START JOB:      " << it->first 
						<< " " << job->time << " " << job->actor 
						<< " " << job->action << "\n";  

			logs = core::string::format() << logs << "    --- END LOOP: " << step << "\n\n";  
			LDEBUG(logs);    

			auto msg = tegia::message::init();
			msg->data["request"] = job->message;

			tegia::message::send( job->actor, job->action, msg );	

			this->jobs.erase(it); 
			this->jobs.insert(std::make_pair( A2Scheduler::parse(job->time), job ));
		}
		else
		{
			step = step + 1;

			// 
			// REM: Обработка накопленной ошибки 
			// 
			// Функция std::this_thread::sleep_for() гарантирует, что поток не будет пробужден 
			// раньше срока, который задан с помощью аргумента функции. Но нет никакой гарантии, 
			// что поток проснется именно в тот момент. Он может проснуться гораздо позже, т.к. 
			// все это зависит от ОС и степени её нагрузки в данный момент.
			// К тому же, на выполнение кода в цикле планировщика также затрачивается некоторое 
			// время. В итоге, если сделать время "засыпания" потока константным (60 секунд), то 
			// со временем будет накапливаться ошибка, измеряемая секундами.
			// Приведенный ниже код корректирует время засыпания исходя из накопленного времени 
			// ошибки
			// 
			
			this->mu.unlock(); 
			std::this_thread::sleep_for( std::chrono::seconds( 60 - curr % 60 ) ); // Засыпаем
			this->mu.lock();

			curr = std::time(NULL);
		}
	}

	_STATUS_ = 200;    
		 
	/////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);

};

