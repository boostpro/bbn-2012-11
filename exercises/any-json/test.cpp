// Copyright Dave Abrahams 2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include "json.hpp"
#include <iostream>

using namespace json;

value dave()
{
    object me;
                                               // {                                    
    me["first"] = string("Dave");              //   "first": "Dave",                   
    me["last"] = string("Abrahams");           //   "last": "Abrahams",                
    me["age"] = 48.2;                          //   "age": 48.2,                       
    me["sex"] = string("M");                   //   "sex": "M",                        
    me["zip code"] = string("02143");          //   "zip code": "02143",               
    me["registered"] = true;                   //   "registered": true,                
    me["catchphrase"] = null();                //   "catchphrase": null                
    array my_interests;                        //   "interests": [                     
    my_interests.push_back(string("Reading")); //       "Reading", 
    my_interests.push_back(string("Biking"));  //       "Biking",
    my_interests.push_back(string("Hacking")); //       "Hacking"                                    
    me["interests"] = my_interests;            //     ]
                                               // }
    return me;
}

int main()
{
    $$ writeme $$
}
