
////////////////////////////////////////////////////////////////////////////////////////////
/** 

	--------------------------------------------
	ROUT: /xml2json
	--------------------------------------------

	--------------------------------------------
	INPUT MESSAGE:
	--------------------------------------------

	{
		"data":
		{
			"file": <string: file path>
		}
	}

	--------------------------------------------
	OUTPUT MESSAGE:
	--------------------------------------------
	
	200

	{
		"status": "ok",
		"data"
		{
			"file": <string: file path>
		}
	}

*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string A2Files::xml_to_json(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	std::string _STATUS_ = "200";
	/////////////////////////////////////////////////////////////////////////////////////

	std::filesystem::path file( message->data["data"]["file"].get<std::string>() );

	std::string xml_file = message->data["data"]["file"].get<std::string>();
	std::string json_file = file.parent_path().string() + "/" + file.stem().string() + ".json";

	std::string xml_str, json_str;
	std::ifstream inf;
	std::ofstream outf;
	std::ostringstream oss;

	inf.open(xml_file);
	outf.open(json_file);
	oss.str("");
	oss << inf.rdbuf();
	xml_str = oss.str();
	inf.close();
	json_str = xml2json(xml_str.c_str());

	outf << json_str;
	outf.close();

	message->data["data"]["file"] = json_file;  
	std::filesystem::remove(xml_file);        

	/////////////////////////////////////////////////////////////////////////////////////
	return _STATUS_;
};   



