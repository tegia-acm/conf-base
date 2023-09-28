/////////////////////////////////////////////////////////////////////////////////////////
/*



*/
/////////////////////////////////////////////////////////////////////////////////////////

std::string A2Example::commit(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	auto responsedata = &(message->data["responce"]["data"]);
	/////////////////////////////////////////////////////////////////////////////////////

	this->commit_lock.lock();
	this->iterator--;

	if(this->iterator == 0)
	{
		auto t_end = std::chrono::high_resolution_clock::now();

		std::cout << "\n====================================" << std::endl;
		std::cout << _OK_TEXT_ << "END CASE [time: " << std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count() << "ms" << "]" << std::endl;
		std::cout << "====================================\n" << std::endl;
	}

	this->commit_lock.unlock();

	/////////////////////////////////////////////////////////////////////////////////////
	return "ok";
};   


