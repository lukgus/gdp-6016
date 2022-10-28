#pragma once

#include <string>

namespace bcrypt 
{
    std::string generateHash(const std::string & password , unsigned int rounds = 10 );
    bool validatePassword(const std::string & password, const std::string & hash);
}
