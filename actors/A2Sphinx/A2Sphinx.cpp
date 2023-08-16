////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONSTRUCTOR / DESTRUCTOR                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "A2Sphinx.h"

A2Sphinx::A2Sphinx(
	const std::string &name, 
	nlohmann::json &data):tegia::actors::actor_base(ACTOR_TYPE, name, data)
{
	//
	// Проверяем запущен ли sphinx search
	//

	auto node_conf = tegia::conf::get("node");
	this->paths.base = tegia::conf::path("base") + "/actors/A2Sphinx";
	this->paths.bin = (*node_conf)["/sphinx/path"_json_pointer]->get<std::string>();
	this->paths.data = (*node_conf)["/sphinx/data"_json_pointer]->get<std::string>();
	this->paths.config = (*node_conf)["/sphinx/configs"_json_pointer]->get<std::string>();

	
	if( std::filesystem::exists( this->paths.data + "/searchd.pid" ) == false)
	{
		std::cout << _ERR_TEXT_ << "Sphinx not load" << std::endl;
		
		//
		// Генерирум конфиг для sphinx и запускаем его
		//

		int ret = system(("bash " +  this->paths.base + "/generate_conf.sh " + this->paths.config + " " + this->paths.data + " " + this->paths.bin).c_str());
	}
	else
	{
		std::cout << "bash " +  this->paths.base + "/generate_conf.sh " + this->paths.config + " " + this->paths.data + " " + this->paths.bin << std::endl;
		std::cout << _OK_TEXT_ << "Sphinx is load" << std::endl;
	}

};

A2Sphinx::~A2Sphinx(){ };

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "actions/create.cpp"
#include "actions/update.cpp"
#include "actions/restart.cpp"


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SUPPORT FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////
