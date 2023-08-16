////////////////////////////////////////////////////////////////////////////////////////////
/** 
 

*/
////////////////////////////////////////////////////////////////////////////////////////////


std::string A2Files::http_test(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	std::string _STATUS_ = "ok";
	/////////////////////////////////////////////////////////////////////////////////////

	auto http = new tegia::http::client();

	http->set_sslp12("/home/igor/tegia/nodes/source.my-check.local/ssl/dit/108013.p12","+9wRdwPK",false);

	int status = http->get(
		"https://ftp.egrul.nalog.ru/EGRIP_405/20.01.2022/EGRIP_2022-01-20_1.zip",
		"/home/igor/EGRIP_2022-01-20_1.zip");
	if(status == 200)
	{
		std::cout << _OK_TEXT_ << "status = " << status << std::endl;
	}
	else
	{
		std::cout << _ERR_TEXT_ << "status = " << status << std::endl;
	}

	delete http;

	/////////////////////////////////////////////////////////////////////////////////////
	return _STATUS_;
};   

