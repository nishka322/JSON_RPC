/* Пока не используется */
#include "XML_1C_integration.h"

#include <iostream>

using json = nlohmann::json;

json handle_post_request(const json& request, int& http_status) {
    json response;

    // Извлечение ID из запроса
    int id = request["params"].value("id", 0);  
    std::cout << "Received ID from request: " << id << std::endl;

    // Проверка на отсутствие или некорректный ID
    if (id == 0) {
        std::cerr << "ID is missing or invalid\n";
        http_status = 400;  
        response = {
                {"jsonrpc", "2.0"},
                {"error", {{"code", -32003}, {"message", "Invalid or missing ID"}}},
                {"id", request["id"]}
        };
        return response;
    }

    // Загрузка XML файла
    tinyxml2::XMLDocument doc;
    std::cout << "Loading XML file from: C:/dev/Clione/MSU/API/Server/DogsXML.xml" << std::endl;
    if (doc.LoadFile("C:/dev/Clione/MSU/API/Server/DogsXML.xml") != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << doc.ErrorStr() << std::endl;
        http_status = 500;
        response = {
                {"jsonrpc", "2.0"},
                {"error", {{"code", -32001}, {"message", "Error loading XML file"}}},
                {"id", request["id"]}
        };
        return response;
    }
    std::cout << "XML file loaded successfully" << std::endl;

    // Поиск корневого элемента
    tinyxml2::XMLElement* rootElement = doc.FirstChildElement("Справочник");
    if (rootElement == nullptr) {
        std::cerr << "Root element not found in XML file" << std::endl;
        http_status = 500;
        response = {
                {"jsonrpc", "2.0"},
                {"error", {{"code", -32002}, {"message", "Root element not found"}}},
                {"id", request["id"]}
        };
        return response;
    }
    std::cout << "Root element found" << std::endl;

    // Поиск элемента Собака с указанным ID
    tinyxml2::XMLElement* dogElement = rootElement->FirstChildElement("Собака");
    bool dogFound = false;
    while (dogElement != nullptr) {
        int elementId = dogElement->IntAttribute("ID");
        std::cout << "Checking dog element with ID: " << elementId << std::endl;
        if (elementId == id) {
            json resultJson = {
                    {"ID", elementId},
                    {"Name", dogElement->Attribute("Имя")},
                    {"Sum", dogElement->IntAttribute("Сумма")},
                    {"Date", dogElement->Attribute("Дата")}
            };
            std::cout << "Found dog with ID: " << id << std::endl;
            response = {
                    {"jsonrpc", "2.0"},
                    {"result", resultJson},
                    {"id", request["id"]}
            };
            http_status = 200;
            dogFound = true;
            break;
        }
        dogElement = dogElement->NextSiblingElement("Собака");
    }

    // Если собака не найдена
    if (!dogFound) {
        std::cout << "No dog found with ID: " << id << std::endl;
        response = {
                {"jsonrpc", "2.0"},
                {"result", nullptr},  // Используем null вместо строки "No dog found"
                {"id", request["id"]}
        };
        http_status = 200;
    }

    return response;
}