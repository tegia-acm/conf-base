/////////////////////////////////////////////////////////////////////////////////////////
/*

	"task":
	{
		"uuid": <uuid>,
		"type": "EMAIL_SEND",
		"data":
		{
			"from": <string>,
			"to": <string>,
			"subject": <string>,
			"template": "mychek-order.tmpl",
			"personalizations":
			{
				<data object>
			}
		}
	}

*/
/////////////////////////////////////////////////////////////////////////////////////////

#include <queue>

#include <tegia/const.h>
#include <tegia/core/string.h>
#include <tegia/core/json.h>


std::queue<std::string> init_message(std::string to, std::string from, std::string subj, std::string message )
{	
	std::queue<std::string> msg;
	std::string s = "To: " + to + "\n";
	msg.push( s );
	s = "From: " + from + "\n";
	msg.push( s );
	s = "Subject: " + subj + "\n";
	msg.push( s );

	s = "Content-Type: text/html; charset=us-ascii\n";
	msg.push( s );
	s = "Mime-version: 1.0\n";
	msg.push( s );

	s = "\n";
	msg.push( s );
	msg.push( message );
	return msg;
}
 
static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
	std::queue<std::string> *msg = (std::queue<std::string>*)userp;
	std::string s;
	if( msg->empty()){
		return 0;
	}
	else{
		s = msg->front();
		msg->pop();
	}
	
	memcpy(ptr, s.c_str(), s.size() );
	return s.size();
};


std::string parse_template(const std::string &template_path, const nlohmann::json &data)
{
	std::u32string result;
	std::u32string tmpl = U"";
	std::string tmpl_line;

    std::ifstream in(template_path); // окрываем файл для чтения
    if (in.is_open())
    {
        while (std::getline(in, tmpl_line))
        {
            tmpl.append(tegia::string::str_to_u32str(tmpl_line));
			tmpl.push_back(U'\n');
        }
    }
    in.close();     // закрываем файл

	int state = 0;
	size_t pos = 0;
	size_t len = tmpl.length();

	std::u32string tag = U"";
	size_t start_tag = 0;
	size_t end_tag = 0;

	while(pos < len)
	{
		switch(state)
		{
			case 0:
			{
				if(tmpl[pos] == U'[')
				{
					state = 1;
				}
				else
				{
					result.push_back(tmpl[pos]);
				}
				pos++;
			}
			break;

			case 1:
			{
				if(tmpl[pos] == U'[')
				{
					state = 10;
					start_tag = pos + 1;
				}
				else
				{
					state = 0;
				}
				pos++;
			}
			break;

			case 10:
			{
				if(tmpl[pos] == U']')
				{
					end_tag = pos;
					state = 20;
				}
				else
				{
					tag.push_back(tmpl[pos]);
				}
				pos++;
			}
			break;

			case 20:
			{
				if(tmpl[pos] == U']')
				{
					state = 30;
				}
				else
				{
					std::cout << _ERR_TEXT_ << "error template" << std::endl;
					exit(0);
				}
			}
			break;

			case 30:
			{
				std::string _tag = tegia::string::u32str_to_str(tag);
				nlohmann::json::json_pointer ptr(_tag);

				if(data.contains(ptr) == true)
				{
					result.append( tegia::string::str_to_u32str(data[ptr].get<std::string>()));
				}
				else
				{
					// TODO: сообщить об ошибке

					std::cout << _ERR_TEXT_ << "not found tag [" << _tag << "]" << std::endl;
					result.append(U"[[]]");
				}

				tag.clear();
				state = 0;

				pos++;
			}
			break;

		}
	}

	return tegia::string::u32str_to_str(result);
};



std::string A2Mailer::smtp(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responcedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	//
	// Параметры
	//	

	std::string _template = message->data["task"]["data"]["template"].get<std::string>();
	std::string _from = message->data["task"]["data"]["from"].get<std::string>();
	std::string _to = message->data["task"]["data"]["to"].get<std::string>();
	std::string _subject = message->data["task"]["data"]["subject"].get<std::string>();
	std::string _message = parse_template("./email_templates/" + _template + ".tmpl",message->data["task"]["data"]["personalizations"]);
	std::queue<std::string> msg = init_message( _to, _from, _subject, _message );

	auto pos = this->emails.find(_from);
	std::string _server = pos->second->smtp.server;
	std::string _password = pos->second->smtp.password;

	/*
	std::cout << "_server   = " << _server << std::endl;
	std::cout << "_password = " << _password << std::endl;

	std::cout << "_template = " << _template << std::endl;
	std::cout << "_from     = " << _from << std::endl;
	std::cout << "_to       = " << _to << std::endl;
	*/

	//
	// Отправка
	//

	CURL *curl;
	CURLcode res;
	struct curl_slist *recipients = NULL;	
	curl = curl_easy_init();
	
	if( curl )
	{
		curl_easy_setopt(curl, CURLOPT_URL, _server.c_str());
		curl_easy_setopt(curl, CURLOPT_USERNAME, _from.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, _password.c_str());
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, _from.c_str() );
		recipients = curl_slist_append(recipients, _to.c_str() );
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
	
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &msg );
	
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		{
			std::string err = curl_easy_strerror(res);
			message->data["task"]["status"] = 500;
			message->data["task"]["error"]["code"] = res;
			message->data["task"]["error"]["info"] = err;
			LERROR("smtp: error send message to '" << _to << "' code = '" << res << "'\n'" << err << "'")
		}
		else
		{
			message->data["task"]["status"] = 200;
			LNOTICE("smtp: success send message to '" << _to << "'")
		}
		
		curl_slist_free_all(recipients);
		curl_easy_cleanup(curl);	
	}
	
	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   
