#ifndef H_EVENTS_ROUTER
#define H_EVENTS_ROUTER

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

// STL 
  #include <string>
  #include <iostream>
  #include <sstream>
  #include <unordered_map>

  #include <tegia/dao/entity.h>



namespace tegia { 
namespace events { 


class router
{
	protected:
		std::unordered_multimap<std::string, nlohmann::json> maps;

	public:   

		
		router() { };
		virtual ~router() {  };


		bool add_subscriber(const std::string &emiter_type, const std::string &emiter_id, const std::string &event_type, nlohmann::json &subscriber)
		{
			std::string event = "/" + emiter_type + "/" + emiter_id + "/" + event_type;

			subscriber["emiter_type"] = emiter_type;
			subscriber["emiter_id"]   = emiter_id;
			subscriber["event_type"]  = event_type;

			this->maps.insert(std::make_pair(event,subscriber));

			return true;
		};


		bool add_subscriber(const std::string &emiter_type, 
								  const std::string &emiter_id, 
								  const std::string &event_type, 
								  const std::string &subscriber_actor,
								  const std::string &subscriber_action)
		{
			std::string event = "/" + emiter_type + "/" + emiter_id + "/" + event_type;

			nlohmann::json subscriber;
			subscriber["emiter_type"] = emiter_type;
			subscriber["emiter_id"]   = emiter_id;
			subscriber["event_type"]  = event_type;
			subscriber["actor"]       = subscriber_actor;
			subscriber["action"]      = subscriber_action;

			this->maps.insert(std::make_pair(event,subscriber));

			return true;
		};



		nlohmann::json find(const std::string &emiter_type, const std::string &emiter_id, const std::string &event_type)
		{
			nlohmann::json data;

			std::vector<std::string> tmp;
			tmp.push_back("/*/*/*");

			tmp.push_back("/*/*/" + event_type);
			tmp.push_back("/*/" + emiter_id + "/*");
			tmp.push_back("/" + emiter_type + "/*/*");

			tmp.push_back("/*/" + emiter_id + "/" + event_type);
			tmp.push_back("/" + emiter_type + "/*/" + event_type);
			tmp.push_back("/" + emiter_type + "/" + emiter_id + "/*");

			tmp.push_back("/" + emiter_type + "/" + emiter_id + "/" + event_type);


			for(auto it = tmp.begin(); it != tmp.end(); it++)
			{
				auto ev = this->maps.find( (*it) );

				if(ev != this->maps.end() )
				{
					 auto range = this->maps.equal_range(ev->first);
					 for(auto it2 = range.first; it2 != range.second; it2++)
					 {
						 data.push_back(it2->second);
					 }
					 //std::cout << (*it) << std::endl;
					 //std::cout << ev->second << std::endl;
					 //std::cout << std::endl;
					 
				}
				else
				{
				//  REM: событие подходящее найдено, но не найдено ни одного обработчика для этого события
					 //std::cout << (*it) << std::endl;
					 //std::cout << "end" << std::endl;
					 //std::cout << std::endl;
				}
			}

			return data;

		};



};	// END router CLASS


}	// END namespace events
}	// END namespace tegia


#endif 

