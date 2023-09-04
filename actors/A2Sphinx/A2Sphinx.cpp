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
	std::cout << _YELLOW_ << std::endl;
	std::cout << data << std::endl;
	std::cout << _BASE_TEXT_ << std::endl;

	//
	// Определяем пути
	//

	this->paths.base = data["path"].get<std::string>();
	this->paths.bin = this->paths.base + "/sphinx-3.1.1/bin";
	this->paths.data = this->paths.base + "/sphinxdata";
	this->paths.config = this->paths.base + "/configs";

	// bash /home/igor/tegia/configurations/conf-base/actors/A2Sphinx/generate_conf.sh /home/igor/tegia/sphinx/configs /home/igor/tegia/sphinx/sphinxdata /home/igor/tegia/sphinx/sphinx-3.1.1/bin
	
	//
	// Проверяем запущен ли sphinx search
	//
	
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
