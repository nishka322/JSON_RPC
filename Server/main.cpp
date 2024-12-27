#include <iostream>
#include <pqxx/pqxx>
#include "tinyxml2.h"
#include "XML_1C_integration.h"
#include "httplib.h"
#include "request_handler.h"
#include <nlohmann/json.hpp>
#include <filesystem>

using namespace httplib;
using namespace tinyxml2;

int main() {
    try {
        // Проверка текущей рабочей директории
        std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

        // Подключение к базе данных
        std::cout << "Connecting to database..." << std::endl;
        pqxx::connection conn("dbname=dogs_db user=postgres password=123456 host=localhost port=5432");
        if (conn.is_open()) {
            std::cout << "Connected to database: " << conn.dbname() << std::endl;
        } else {
            std::cerr << "Failed to connect to database" << std::endl;
            return 1;
        }

        // Запрос для вывода всех данных из таблицы dogs
        pqxx::work txn(conn);
        pqxx::result res = txn.exec("SELECT * FROM dogs");

        // Вывод таблицы собак
        std::cout << "Таблица собак:" << std::endl;
        std::cout << "ID | Name   | Breed       | Age" << std::endl;
        std::cout << "------------------------------" << std::endl;

        for (const auto& row : res) {
            std::cout << row["id"].as<int>() << " | "
                      << row["name"].as<std::string>() << " | "
                      << row["breed"].as<std::string>() << " | "
                      << row["age"].as<int>() << std::endl;
        }

        // Завершаем транзакцию после вывода собак
        txn.commit();
        std::cout << "Transaction committed successfully." << std::endl;

        // Путь к XML файлу
        const std::string xmlFilePath = "C:/dev/Clione/MSU/API/Server/DogsXML.xml";

        // Проверка работы с XML файлом
        tinyxml2::XMLDocument doc;
        std::cout << "Loading XML file from: " << xmlFilePath << std::endl;
        if (doc.LoadFile(xmlFilePath.c_str()) != tinyxml2::XML_SUCCESS) {
            std::cerr << "Error loading XML file: " << doc.ErrorStr() << std::endl;
            return 1;
        }
        std::cout << "XML file loaded successfully" << std::endl;

        tinyxml2::XMLElement* rootElement = doc.FirstChildElement("Справочник");
        if (rootElement == nullptr) {
            std::cerr << "Root element not found in XML file" << std::endl;
            return 1;
        }
        std::cout << "Root element found" << std::endl;

        // Вывод данных из XML файла
        tinyxml2::XMLElement* dogElement = rootElement->FirstChildElement("Собака");
        while (dogElement != nullptr) {
            nlohmann::json dogData = {
                    {"ID", dogElement->Attribute("ID")},
                    {"Name", dogElement->Attribute("Имя")},
                    {"Summ", dogElement->Attribute("Сумма")},
                    {"Date", dogElement->Attribute("Дата")}
            };
            std::cout << "Dog data: " << dogData.dump() << std::endl;
            dogElement = dogElement->NextSiblingElement("Собака");
        }

        // Создание и запуск HTTP сервера
        std::cout << "Creating and starting HTTP server..." << std::endl;
        Server svr;

        svr.Post("/jsonrpc", [&conn](const Request& req, Response& res) {
            std::cout << "Received request body: " << req.body << std::endl;

            nlohmann::json request;
            int http_status = 200;

            try {
                request = nlohmann::json::parse(req.body);
                std::cout << "Parsed JSON request: " << request.dump() << std::endl;
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
                res.status = 400;
                res.set_content("{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32700, \"message\": \"Parse error\"}, \"id\": null}", "application/json");
                return;
            }

            nlohmann::json response;
            try {
                // Обработка запроса
                response = handle_db_request_with_status(request, conn, http_status);
                std::cout << "Response generated: " << response.dump() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Exception while processing request: " << e.what() << std::endl;
                response = {
                        {"jsonrpc", "2.0"},
                        {"error", {{"code", -32000}, {"message", "Internal server error"}}},
                        {"id", request.value("id", nullptr)}
                };
                http_status = 500;
            }

            res.status = http_status;
            res.set_content(response.dump(), "application/json");
            std::cout << "Response sent to client with status: " << http_status << std::endl;
        });

        std::cout << "Server started at http://localhost:8080\n";
        svr.listen("localhost", 8080);
    } catch (const std::exception& e) {
        std::cerr << "Exception in main: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
