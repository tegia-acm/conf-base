#include <tegia/db/mysql/mysql.h>


bool A2Names::load_names(void)
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Загружаем имена из БД
	//
	/////////////////////////////////////////////////////////////////////////////////////////////

	auto res = tegia::mysql::query("core","SELECT ru_name, sex FROM names");
	int count = res->count();
	for(int i = 0; i < count; i++)
	{
		std::string name = res->get("names::ru_name",i);
		int sex = core::cast<int>(res->get("names::sex",i));

		this->sex_map.insert({name,sex});	
	}
	delete res;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Исключительные ситуации из ЕГРЮЛ
	//
	/////////////////////////////////////////////////////////////////////////////////////////////


	this->sex_map.insert({"Микко петтери",9});	// ogrn = 1127847222370
	this->sex_map.insert({"Чжихуа",9});	         // ogrn = 1127901000831
																//        Предположительно, это не имя, а китайская фамилия
																//        https://ru.wikipedia.org/wiki/%D0%9B%D1%8E_%D0%A7%D0%B6%D0%B8%D1%85%D1%83%D0%B0
	this->sex_map.insert({"Карши",9});	         // ogrn = 1142651011359
	this->sex_map.insert({"Ювиналий",9});        // ogrn = 1115190025500
															 	//        Предположительно, опечатка. Правильное имя - Ювеналий
																//        https://ru.wikipedia.org/wiki/%D0%AE%D0%B2%D0%B5%D0%BD%D0%B0%D0%BB%D0%B8%D0%B9
	this->sex_map.insert({"Ашотик",9});          // ogrn = 1124300000010
																//        Предположительно, опечатка. Правильное имя - Ашот
	this->sex_map.insert({"Артур тадеуш",9});    // ogrn = 1126732006114
	this->sex_map.insert({"Кинилл",9});          // ogrn = 1127847153961
																//        Предположительно, опечатка. Правильное имя - Кирилл
	this->sex_map.insert({"Киреев",9});          // ogrn = 1125543020734
	this->sex_map.insert({"Калина",9});          // ogrn = 1125906002320
																//        Считается женским именем (https://imya.com/name/8380),
																//        но в данном ОГРН по контексту мужское имя. Необходимо уточнение у филологов
	this->sex_map.insert({"Мохссен",9});         // ogrn = 1127746432065
																//        Предположительно, опечатка. Правильное имя - Мохсен
																//        https://imya.com/name/29406
	this->sex_map.insert({"Алексис",9});         // ogrn = 1127746310438
																//        Может быть как мужским именем, так и женским
																//        M: https://imya.com/name/7282
																//        F: https://imya.com/name/18074
	this->sex_map.insert({"Рауфат",9});          // ogrn = 1120200001844
																//        Такое имя не найдено
																//        Но есть имя Руфат: https://imya.com/name/39551
	this->sex_map.insert({"Хади",9});            // ogrn = 1120200001844
																//        Может быть как мужским именем, так и женским
																//        F: https://imya.com/name/18672
																//        M: https://imya.com/name/34490
	this->sex_map.insert({"Вогиз",9});           // ogrn = 1127746680467
																//        Предположительно, опечатка. Правильное имя - Вагиз
																//        https://imya.com/name/21885
	this->sex_map.insert({"Мафедар",9});         // ogrn = 1126164001259
																//        Такое имя не найдено
	this->sex_map.insert({"Жамилахон",9});       // ogrn = 1127746748964
																//        Такое имя не найдено
	this->sex_map.insert({"Чжо",9});             // ogrn = 1127847267074
																//        Такое имя не найдено
	this->sex_map.insert({"Минь хыонг",9});      // ogrn = 1080277005050
																//        Такое имя не найдено
	this->sex_map.insert({"Автанден",9});        // ogrn = 1085543036622
																//        Предположительно, опечатка. Правильное имя - Автандил
																//        https://ru.wikipedia.org/wiki/Автандил
	this->sex_map.insert({"Анник",9});           // ogrn = 1087746820083
																//        Такое имя не найдено
	this->sex_map.insert({"Куок тьуен",9});      // ogrn = 1087746792088
																//        Такое имя не найдено
	this->sex_map.insert({"Асланди",9});         // ogrn = 
																//        Такое имя не найдено
	this->sex_map.insert({"Хананя",9});          // ogrn = 1047796545411
																//        Такое имя не найдено







// Трешнина

	this->sex_map.insert({"А",9});               // ogrn = 1125476052790
	this->sex_map.insert({"А.",9});              // ogrn = 1147746490011
	this->sex_map.insert({"Аа",9});              // ogrn = 1127746687584
	this->sex_map.insert({"А.а.",9});            // ogrn = 

	this->sex_map.insert({"Б",9});               // ogrn = 
	this->sex_map.insert({"Б.",9});              // ogrn = 



	this->sex_map.insert({"В.",9});              // ogrn = 1127847273168
	this->sex_map.insert({"В",9});               // ogrn = 

	this->sex_map.insert({"Г.",9});              // ogrn = 1127847483488
	this->sex_map.insert({"Г",9});               // ogrn = 

	this->sex_map.insert({"Д.",9});              // ogrn = 

	this->sex_map.insert({"Е",9});               // ogrn = 
	this->sex_map.insert({"Е.",9});              // ogrn = 

	this->sex_map.insert({"Ж.",9});              // ogrn = 

	this->sex_map.insert({"З.",9});              // ogrn = 

	this->sex_map.insert({"И.",9});              // ogrn = 
	this->sex_map.insert({"И",9});               // ogrn = 

	this->sex_map.insert({"К",9});               // ogrn = 
	this->sex_map.insert({"К.",9});              // ogrn = 

	this->sex_map.insert({"Л",9});               // ogrn = 
	this->sex_map.insert({"Л.",9});              // ogrn = 1127847181318

	this->sex_map.insert({"М.",9});              // ogrn = 1127847273168
	this->sex_map.insert({"М",9});               // ogrn = 

	this->sex_map.insert({"Н.",9});              // ogrn = 1147746490011
	this->sex_map.insert({"Н",9});               // ogrn = 5067746993749

	this->sex_map.insert({"О.",9});              // ogrn = 

	this->sex_map.insert({"П.",9});              // ogrn = 
	this->sex_map.insert({"П",9});               // ogrn = 

	this->sex_map.insert({"Р",9});               // ogrn = 
	this->sex_map.insert({"Р.",9});              // ogrn = 

	this->sex_map.insert({"С.",9});              // ogrn = 1127847273168
	this->sex_map.insert({"С",9});               // ogrn = 1087746011770

	this->sex_map.insert({"Т",9});               // ogrn = 
	this->sex_map.insert({"Т.",9});              // ogrn = 

	this->sex_map.insert({"Ф.",9});              // ogrn = 

	this->sex_map.insert({"Ю.",9});              // ogrn = 1127847273168
	this->sex_map.insert({"Ю",9});               // ogrn = 


	this->sex_map.insert({"Геннадьевич",9});     // ogrn = 1087746047729
	this->sex_map.insert({"Инейт",9});           // ogrn = 1087746419639





	return true;
};