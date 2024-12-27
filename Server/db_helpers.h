#ifndef DB_HELPERS_HPP
#define DB_HELPERS_HPP

#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include <iostream>

nlohmann::json get_dog_by_id(pqxx::connection& conn, int id);
nlohmann::json get_dogs_by_breed(pqxx::connection& conn, const std::string& breed);
nlohmann::json get_dogs_by_age(pqxx::connection& conn, int age);
nlohmann::json get_dog_by_name(pqxx::connection& conn, const std::string& name);
bool update_dog(pqxx::connection& conn, int id, const std::string& name, const std::string& breed, int age);

#endif // DB_HELPERS_HPP
