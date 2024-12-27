#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <pqxx/pqxx>
#include <nlohmann/json.hpp>

nlohmann::json handle_db_request(const nlohmann::json& request, pqxx::connection& conn);
nlohmann::json handle_db_request_with_status(const nlohmann::json& request, pqxx::connection& conn, int& http_status);

#endif // REQUEST_HANDLER_HPP
