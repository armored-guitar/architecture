#include "person.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>
#include <fstream>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database 
{
    void Person::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<std::string> hints = database::Database::get_all_hints();
            for (const std::string &hint : hints) {
                //*
                std::string drop_cmd = "DROP TABLE IF EXISTS Person;";
                drop_cmd += hint;
                Statement drop_stmt(session);
                drop_stmt << drop_cmd;
                drop_stmt.execute();

                //*/

                // (re)create table

                Statement create_stmt(session);
                create_stmt << "CREATE TABLE IF NOT EXISTS `Person` ("
                            << "`login` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                            << "`first_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                            << "`last_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                            << "`age` INT NOT NULL,"
                            << "PRIMARY KEY (`login`), INDEX name (`last_name`,`first_name`));"
                            << hint,
                        now;
            }
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Person::preload(const std::string &file)
    {
        try
        {

            std::string json;
            std::ifstream is(file);
            std::istream_iterator<char> eos;
            std::istream_iterator<char> iit(is);
            while (iit != eos)
                json.push_back(*(iit++));
            is.close();

            Poco::JSON::Parser parser;
            // check that we read the json
            if (json.empty())
            {
                std::cout << "Empty json. Check that the file exists." << std::endl;
            }
            Poco::Dynamic::Var result = parser.parse(json);
            Poco::JSON::Array::Ptr arr = result.extract<Poco::JSON::Array::Ptr>();

            size_t i{0};
            auto array_size = arr->size();
            for (i = 0; i < array_size; ++i)
            {
                Poco::JSON::Object::Ptr object = arr->getObject(i);
                std::string login = object->getValue<std::string>("login");
                std::string first_name = object->getValue<std::string>("first_name");
                std::string last_name = object->getValue<std::string>("last_name");
                unsigned short age = object->getValue<unsigned short>("age");
                database::Person person;
                person.age() = age;
                person.login() = login;
                person.first_name() = first_name;
                person.last_name() = last_name;
                person.save_to_db();
            }

            std::cout << "Inserted " << i << " records" << std::endl;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Person::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("login", _login);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("age", _age);

        return root;
    }

    // Person Person::fromJSON(const std::string &str)
    // {
    //     Person person;
    //     Poco::JSON::Parser parser;
    //     Poco::Dynamic::Var result = parser.parse(str);
    //     Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

    //     person.login() = object->getValue<std::string>("login");
    //     person.first_name() = object->getValue<std::string>("first_name");
    //     person.last_name() = object->getValue<std::string>("last_name");
    //     person.age() = object->getValue<unsigned short>("age");
    //     return person;
    // }

    Person Person::fingByLogin(std::string login)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::string sharding_hint = database::Database::sharding_hint(login);
            Person p;

            std::string select_str = "SELECT login, first_name, last_name, age  FROM Person where login=?";
            select_str += sharding_hint;
            select << select_str,
                into(p._login),
                into(p._first_name),
                into(p._last_name),
                into(p._age),
                use(login),
                range(0, 1); //  iterate over result set one row at a time
            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst()) throw std::logic_error("not found");

            return p;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<Person> Person::findByName(std::string first_name, std::string last_name)
    {
        try
        {
            first_name+="%";
            last_name+="%";
            std::vector<Person> result;
            std::vector<std::string> hints = database::Database::get_all_hints();
            for (const std::string &hint : hints){
                Person p;
                Poco::Data::Session session = database::Database::get().create_session();
                Statement select(session);
                select << "SELECT login, first_name, last_name, age FROM Person "
                          "where first_name LIKE ? and last_name LIKE ? " + hint,
                        into(p._login),
                        into(p._first_name),
                        into(p._last_name),
                        into(p._age),
                        use(first_name),
                        use(last_name),
                        range(0, 1);
                select.execute();

                while (!select.done())
                {
                    select.execute();
                    result.push_back(p);
                }
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

   
    void Person::save_to_db()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            std::string sharding_hint = database::Database::sharding_hint(_login);
            std::string select_str = "INSERT INTO Person (login, first_name, last_name, age) VALUES(?, ?, ?, ?)";
            select_str += sharding_hint;

            insert << select_str,
                    use(_login),
                    use(_first_name),
                    use(_last_name),
                    use(_age);

            insert.execute();
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    const std::string &Person::get_login() const
    {
        return _login;
    }

    const std::string &Person::get_first_name() const
    {
        return _first_name;
    }

    const std::string &Person::get_last_name() const
    {
        return _last_name;
    }

    const unsigned short &Person::get_age() const
    {
        return _age;
    }


    std::string &Person::login()
    {
        return _login;
    }

    std::string &Person::first_name()
    {
        return _first_name;
    }

    std::string &Person::last_name()
    {
        return _last_name;
    }

    unsigned short &Person::age()
    {
        return _age;
    }
}
