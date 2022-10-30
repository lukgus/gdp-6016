#include "bcrypt.h"
#include "bcrypt/BCrypt.hpp"
#include <iostream>
#include <string>

void FirstHashLibraryTest()
{
    // Account Creation
    std::string salt = "fash9873";
    std::string user_input_password = "top_secret";
    std::string salt_password = salt/*generated*/ + user_input_password;
    std::string hash = bcrypt::generateHash(salt_password, 13);
    // Store [ salt | hash ] in the database

    // Account Authentication
    // Retrieve [salt | hash] from the database
    std::cout << "Hash: " << hash << std::endl;
    std::cout << "\"" << user_input_password << "\" : " << bcrypt::validatePassword(
        salt/*from DB*/ + user_input_password, hash) << std::endl;

    std::cout << "\"wrong\" : " << bcrypt::validatePassword("wrong", hash) << std::endl;
}

void SecondHashLibraryTest()
{
    std::string password = "test";
    std::string hash = BCrypt::generateHash(password);

    std::cout << "Hash: " << hash << std::endl;
    std::cout << "\"" << password << "\" : " << BCrypt::validatePassword(password, hash) << std::endl;
    std::cout << "\"test1\" : " << BCrypt::validatePassword("test1", hash) << std::endl;
}

int main(int argc, char** argv)
{
    FirstHashLibraryTest();
    printf("\n--------------------------------------\n\n");
    SecondHashLibraryTest();
    return 0;
}

