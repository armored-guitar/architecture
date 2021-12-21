#ifndef PERSONHANDLER_H
#define PERSONHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../database/person.h"

class PersonHandler : public HTTPRequestHandler
{
private:
    bool check_name(const std::string &name, std::string &reason)
    {
        if (name.length() < 3)
        {
            reason = "Name must be at leas 3 signs";
            return false;
        }

        if (name.find(' ') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }

        if (name.find('\t') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }

        return true;
    };

    bool check_age(const unsigned short &age, std::string &reason)
    {
        if (age <= 18)
        {
            reason = "Age shouldn't be less than 18";
            return false;
        }

        if (age > 150)
        {
            reason = "Age should be less than 150";
            return false;
        }

        return true;
    };

public:
    PersonHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();
        if (request.getMethod() == "GET")
        {
            if (form.has("login"))
            {
                std::string login = form.get("login");
                try
                {
                    database::Person result = database::Person::fingByLogin(login);
                    Poco::JSON::Stringifier::stringify(result.toJSON(), ostr);
                    return;
                }
                catch (...)
                {
                    ostr << "{ \"result\": false , \"reason\": \"not found\" }";
                    return;
                }
            }
            else if (form.has("first_name") && form.has("last_name"))
            {
                try
                {
                    std::string  fn = form.get("first_name");
                    std::string  ln = form.get("last_name");
                    auto results = database::Person::findByName(fn,ln);
                    Poco::JSON::Array arr;
                    for (auto s : results)
                        arr.add(s.toJSON());
                    Poco::JSON::Stringifier::stringify(arr, ostr);
                }
                catch (...)
                {
                    ostr << "{ \"result\": false , \"reason\": \"not found\" }";
                    return;
                }
                return;
            }
        } else if (request.getMethod() == "POST") {
            if (form.has("login"))
                if (form.has("first_name"))
                    if (form.has("last_name"))
                        if (form.has("age"))
                        {
                            std::string login = form.get("login");
                            std::string first_name = form.get("first_name");
                            std::string last_name = form.get("last_name");
                            unsigned short age = atol(form.get("age").c_str());

                            bool check_result = true;
                            std::string message;
                            std::string reason;

                            if (!check_name(first_name, reason))
                            {
                                check_result = false;
                                message += reason;
                                message += "<br>";
                            }

                            if (!check_name(last_name, reason))
                            {
                                check_result = false;
                                message += reason;
                                message += "<br>";
                            }

                            if (!check_name(login, reason))
                            {
                                check_result = false;
                                message += reason;
                                message += "<br>";
                            }

                            if (!check_age(age, reason))
                            {
                                check_result = false;
                                message += reason;
                                message += "<br>";
                            }

                            if (check_result)
                            {
                                try
                                {
                                    database::Person existing_person = database::Person::fingByLogin(login);
                                    ostr << "{ \"result\": false , \"reason\": \" login already in database\" }";
                                    return;
                                }
                                catch (...)
                                {
                                    database::Person person;
                                    person.age() = age;
                                    person.login() = login;
                                    person.first_name() = first_name;
                                    person.last_name() = last_name;
                                    person.send_to_queue();
                                        ostr << "{ \"result\": true }";
                                        return;
                                }

                            }
                            else
                            {
                                ostr << "{ \"result\": false , \"reason\": \"" << message << "\" }";
                                return;
                            }
                        }
        }
    }

private:
    std::string _format;
};
#endif // !PERSONHANDLER_H
