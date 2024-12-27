#ifndef SERVER_XML_1C_INTEGRATION_H
#define SERVER_XML_1C_INTEGRATION_H

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <tinyxml2.h>
#include <pqxx/pqxx>
#include <iostream>
#include <fstream>

using namespace httplib;
using namespace tinyxml2;
using json = nlohmann::json;

json handle_post_request(const json& request, int& http_status);
json handle_xml_request(const json& request, pqxx::connection& conn, int& http_status);

#endif // SERVER_XML_1C_INTEGRATION_H
