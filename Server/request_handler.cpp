#include "request_handler.h"
#include "db_helpers.h"
#include <nlohmann/json.hpp>
#include "XML_1C_integration.h"

using json = nlohmann::json;

json handle_db_request_with_status(const json& request, pqxx::connection& conn, int& http_status) {
    const std::string method = request.value("method", "");
    json response;

    // Устанавливаем статус по умолчанию
    http_status = 200;

    // Функция для установки ответа об ошибке
    auto set_error_response = [&](int status, int code, const std::string& message) {
        http_status = status;
        response = {
                {"jsonrpc", "2.0"},
                {"error", {
                                    {"code", code},
                                    {"message", message}
                            }},
                {"id", request.contains("id") ? request["id"] : nullptr}  // Если "id" есть, берем его, иначе используем null
        };
        std::cerr << "Sending response: " << response.dump() << std::endl;
        std::cerr << "Error: " << message << std::endl;
    };

    // Вспомогательная функция для извлечения параметров из запроса
    auto get_param = [&](const json& params, const std::string& key, const json& default_value = nullptr) -> json {
        if (params.is_array() && !params.empty()) {
            for (const auto& param : params) {
                if (param.contains(key)) {
                    return param[key];
                }
            }
        } else if (params.is_object() && params.contains(key)) {
            return params[key];
        }
        return default_value;
    };

    try {
        if (method == "get_dog_by_id") {
            const int id = get_param(request["params"], "id", 0).get<int>();
            const json result = get_dog_by_id(conn, id);
            if (result.empty()) {
                set_error_response(404, -32602, "Dog not found");
            } else {
                response = {
                        {"jsonrpc", "2.0"},
                        {"result", result},
                        {"id", request.contains("id") ? request["id"] : nullptr}
                };
            }
        } else if (method == "get_dogs_by_breed") {
            const std::string breed = get_param(request["params"], "breed", "").get<std::string>();
            const json result = get_dogs_by_breed(conn, breed);
            if (result.empty()) {
                set_error_response(404, -32602, "No dogs found for the given breed");
            } else {
                response = {
                        {"jsonrpc", "2.0"},
                        {"result", result},
                        {"id", request.contains("id") ? request["id"] : nullptr}
                };
            }
        } else if (method == "get_dogs_by_age") {
            const int age = get_param(request["params"], "age", 0).get<int>();
            const json result = get_dogs_by_age(conn, age);
            if (result.empty()) {
                set_error_response(404, -32602, "No dogs found for the given age");
            } else {
                response = {
                        {"jsonrpc", "2.0"},
                        {"result", result},
                        {"id", request.contains("id") ? request["id"] : nullptr}
                };
            }
        } else if (method == "get_dog_by_name") {
            const std::string name = get_param(request["params"], "name", "").get<std::string>();
            const json result = get_dog_by_name(conn, name);
            if (result.empty()) {
                set_error_response(404, -32602, "Dog not found by name");
            } else {
                response = {
                        {"jsonrpc", "2.0"},
                        {"result", result},
                        {"id", request.contains("id") ? request["id"] : nullptr}
                };
            }
        } else if (method == "update_dog") {
            const int id = get_param(request["params"], "id", 0).get<int>();
            const std::string name = get_param(request["params"], "name", "").get<std::string>();
            const std::string breed = get_param(request["params"], "breed", "").get<std::string>();
            const int age = get_param(request["params"], "age", 0).get<int>();

            const bool success = update_dog(conn, id, name, breed, age);
            if (success) {
                response = {
                        {"jsonrpc", "2.0"},
                        {"result", "Dog updated successfully"},
                        {"id", request.contains("id") ? request["id"] : nullptr}
                };
            } else {
                set_error_response(404, -32602, "Dog not found for update");
            }
        } else if (method == "get_dog_info") {
            const int id = get_param(request["params"], "id", 0).get<int>();
            const json result = get_dog_by_id(conn, id);

            if (result.empty()) {
                set_error_response(404, -32602, "Dog not found");
            } else {
                response = {
                        {"jsonrpc", "2.0"},
                        {"result", result},
                        {"id", request.contains("id") ? request["id"] : nullptr}
                };
            }
        } else {
            set_error_response(400, -32601, "Method not found");
        }
    } catch (const std::exception& e) {
        set_error_response(500, -32603, "Internal server error: " + std::string(e.what()));
    }

    std::cout << "Sending response: " << response.dump() << std::endl;
    return response;
}
