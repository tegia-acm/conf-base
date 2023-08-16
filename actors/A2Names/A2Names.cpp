////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONSTRUCTOR / DESTRUCTOR                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "A2Names.h"

A2Names::A2Names(
	const std::string &name, 
	nlohmann::json &data):tegia::actors::actor_base(ACTOR_TYPE, name, data)
{ 
	// this->load_names();
}; 


A2Names::~A2Names() { };

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


std::string A2Names::transform(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	int _STATUS_ = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	std::string file_name = route_params["file"].get<std::string>();


	auto _config = tegia::conf::get("base");	
	std::string path = (*_config)["path"]->get<std::string>();

	std::cout << "path = " << path << std::endl;

	// Узнаем путь до конфигурации
	/*std::string path = "";
	nlohmann::json configurations = context::node()->config["configurations"];
	for(auto it = configurations.begin(); it != configurations.end(); it++)
	{
		if( (*it)["name"].get<std::string>() == "base")
		{
			path = (*it)["path"].get<std::string>();
			break;
		}
	}*/
	
	// Загружаем файл

	std::string line;
	std::ifstream base_file (path + "/data/" + file_name + ".txt");
	std::ofstream transform_file (path + "/data/" + file_name + "2.txt");

	if (base_file.is_open())
	{
		while ( getline (base_file,line) )
		{
			//std::cout << line << '\n';
			auto names = core::explode(line, " ");
			for(auto it = names.begin(); it != names.end(); it++)
			{
				transform_file << "this->sex_map.insert({\"";
				transform_file << (*it);
				//transform_file << "\n";
				transform_file << "\",1});\n";
			}
		}
		base_file.close();
		transform_file.close();
	}

	 _STATUS_ = 200;

	/////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};   




std::string A2Names::get_sex(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	int _STATUS_ = 0;
	/////////////////////////////////////////////////////////////////////////////////////

	std::string name = route_params["name"].get<std::string>();

	auto pos = this->sex_map.find(name);
	if(pos != this->sex_map.end())
	{
		message->data["name"]["sex"] = pos->second;
		message->data["name"]["name"] = name;
		message->data["name"]["status"] = 200;

		_STATUS_ = 200;
	} 
	else
	{
		message->data["name"]["sex"] = 0;
		message->data["name"]["name"] = name;
		message->data["name"]["status"] = 404;

		this->unknown_names.push_back(name);
      core::json::save("./unknown_names.json",this->unknown_names);

		_STATUS_ = 404;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	return core::cast<std::string>(_STATUS_);
};   


std::string A2Names::test(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	/////////////////////////////////////////////////////////////////////////////////////

	std::cout << _OK_TEXT_ << " A2Names::test" << std::endl;

	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   


std::string A2Names::test1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	/////////////////////////////////////////////////////////////////////////////////////

	std::cout << _ERR_TEXT_ << " A2Names::test1" << std::endl;

	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SUPPORT FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "support/load_names.cpp"


