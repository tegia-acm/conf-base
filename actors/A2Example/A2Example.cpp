////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONSTRUCTOR / DESTRUCTOR                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "A2Example.h"


A2Example::A2Example(
	const std::string &name, 
	nlohmann::json &data)
	: tegia::actors::actor_base(ACTOR_TYPE, name ,data)
{ 


}; 

A2Example::~A2Example() { };

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


#include "actions/run.cpp"

#include "actions/iterator_1.cpp"
#include "actions/iterator_2.cpp"

#include "actions/action.cpp"
#include "actions/commit.cpp"


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SUPPORT FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


