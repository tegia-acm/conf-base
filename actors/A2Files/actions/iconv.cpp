////////////////////////////////////////////////////////////////////////////////////////////
/** 

	 --------------------------------------------
	 ROUT: /iconv
	 --------------------------------------------

	 --------------------------------------------
	 INPUT MESSAGE:
	 --------------------------------------------

	 {
		 "data":
		 {
			 "file": <string: file path>,
			 "charset":
			 {
				 "from": <string: charset name>
				 "to": <string: charset name>
			 }
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
			 "file": <string>,
		 }
	 }

*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string A2Files::iconv(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	std::string _STATUS_ = "200";
	/////////////////////////////////////////////////////////////////////////////////////

	std::filesystem::path file(message->data["data"]["file"].get<std::string>());
 
	std::string old_filename = message->data["data"]["file"].get<std::string>();
	std::string new_filename = file.parent_path().string() + "/utf-8." + file.filename().string();

	std::string charset_from = message->data["data"]["charset"]["from"].get<std::string>();
	std::string charset_to   = message->data["data"]["charset"]["to"].get<std::string>();

	//	Перекодировать  
		std::string iconv_str = "iconv -f " + charset_from + " -t " + charset_to + " " + old_filename + " > " + new_filename;
		// std::cout << iconv_str << std::endl;
		int sys = system(iconv_str.c_str());    
  
	message->data["data"]["file"] = new_filename;
	message->data["status"] = "ok";

	std::filesystem::remove(old_filename);

	/////////////////////////////////////////////////////////////////////////////////////
	return _STATUS_;
};   

