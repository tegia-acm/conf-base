////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONSTRUCTOR / DESTRUCTOR                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "A2Files.h"

A2Files::A2Files(
	const std::string &name, 
	nlohmann::json &data):tegia::actors::actor_base(ACTOR_TYPE, name, data)
{
	this->http_load_validator.set_root_schema(R"json(
	{
		"$schema": "http://json-schema.org/draft-07/schema#",
		"title": "HTTP LOAD TASK schema",
		"type": "object",
		"properties": 
		{
			"task":
			{
				"type": "object",
				"properties":
				{
					"uuid":
					{
						"description": "Уникальный идентификатор задачи",
						"type": "string",
						"pattern": "[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}"
					},
					"type":
					{
						"description": "Тип задачи",
						"type": "string",						
						"pattern": "HTTP_LOAD"
					},
					"data":
					{
						"type": "object",
						"properties":
						{
							"url":
							{
								"description": "Ссылка, по которой нужно скачать файл",
								"type": "string"								
							},
							"filename":
							{
								"description": "Локальный путь, по котороу нужно сохранить скачанный файл",
								"type": "string"								
							},
							"ssl":
							{
								"type": "object",
								"properties":
								{
									"cert":
									{
										"description": "Путь к файлу с ssl-сертификатом в формате p12",
										"type": "string"
									},
									"password":
									{
										"description": "Пароль от файла с ssl-сертификатом в формате p12",
										"type": "string"										
									},
									"is_check":
									{
										"type": "boolean"
									}
								},
								"required": ["cert", "password", "is_check"]
							}
						},
						"required": ["url", "filename"]
					}
				},
				"required": ["uuid", "type", "data"]
			}
		},
		"required": ["task"]
	})json"_json);
}; 

A2Files::~A2Files() { };

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "actions/http_load.cpp"
#include "actions/http_load2.cpp"
#include "actions/unzip.cpp"
#include "actions/iconv.cpp"
#include "actions/xml2json.cpp"

#include "actions/http_test.cpp"


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SUPPORT FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


