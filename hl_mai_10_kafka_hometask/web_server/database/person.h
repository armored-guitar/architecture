#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace database
{
    class Person{
        private:
            std::string _login;
            std::string _first_name;
            std::string _last_name;
            unsigned short _age;

        public:
            static Person fromJSON(const std::string &json);
            const std::string    &get_login() const;
            const std::string    &get_first_name() const;
            const std::string    &get_last_name() const;
            const unsigned short &get_age() const;

            std::string &login();
            std::string &first_name();
            std::string &last_name();
            unsigned short        &age();

            static void init();
            static Person fingByLogin(std::string login);
            static std::vector<Person> findByName(std::string first_name,std::string last_name);
            void save_to_db();
            Poco::JSON::Object::Ptr toJSON() const;
            void send_to_queue();


    };
}

#endif