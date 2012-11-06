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
    me["first"] = "Dave";              //   "first": "Dave",                     
    me["last"] = "Abrahams";           //   "last": "Abrahams",                
    me["age"] = 48.2;                  //   "age": 48.2,                               
    me["sex"] = "M";                   //   "sex": "M",                        
    me["zip code"] = "02143";          //   "zip code": "02143",               
    me["registered"] = true;           //   "registered": true,                        
    me["catchphrase"] = null();        //   "catchphrase": null
    array my_interests;                //   "interests": [
    my_interests.push_back("Reading"); //       "Reading", 
    my_interests.push_back("Biking");  //       "Biking",
    my_interests.push_back("Hacking"); //       "Hacking"                                    
    me["interests"] = my_interests;    //     ]
                                       // }                              
    return me;
}

int main()
{
    std::cout << dave() << std::endl;
}
