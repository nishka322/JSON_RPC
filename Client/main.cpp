#include <iostream>
#include <cpr/cpr.h>
#include "json.hpp"

using json = nlohmann::json;

// Перечисление для типа запроса
enum class RequestType {
    GetDogById = 1,    // Получение собаки по ID
    GetDogsByBreed,    // Получение собак по породе
    GetDogsByAge,      // Получение собак по возрасту
    GetDogByName,      // Получение собаки по имени
    UpdateDog,         // Обновление данных о собаке
    GetDogsXMLInfo,    // Получение информации о собаках из XML
    Exit = 0           // Выход из программы
};

bool is_number(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

int main() {
    int option;

    // Бесконечный цикл
    while (true) {
        // Меню для выбора типа запроса
        std::cout << "Choose request type:\n";
        std::cout << "1. Get dog by ID\n";
        std::cout << "2. Get dogs by breed\n";
        std::cout << "3. Get dogs by age\n";
        std::cout << "4. Get dog by name\n";
        std::cout << "5. Update dog\n";  // Добавлен пункт для обновления собаки
        std::cout << "6. Get dogs XML info\n"; // Новый пункт для получения информации из XML
        std::cout << "0. Exit\n";
        std::cout << "Enter option: ";
        std::cin >> option;

        RequestType request_type = static_cast<RequestType>(option);

        if (request_type == RequestType::Exit) {
            std::cout << "Exiting the program.\n";
            break;
        }

        json request;

        switch (request_type) {
            case RequestType::GetDogById: {
                int id;
                std::cout << "Enter dog ID: ";
                std::cin >> id;

                request = {
                        {"jsonrpc", "2.0"},
                        {"method", "get_dog_info"},
                        {"params", {{"id", id}}},
                        {"id", 1}
                };
                break;
            }

            case RequestType::GetDogsXMLInfo: {
                int id;
                std::cout << "Enter dog ID to get XML info: ";
                std::cin >> id;

                request = {
                        {"jsonrpc", "2.0"},
                        {"method", "get_dog_info"},
                        {"params", {{"id", id}}},
                        {"id", 6}
                };
                break;
            }
            default: {
                std::cout << "Invalid option selected.\n";
                continue;
            }
        }

        // Сериализация JSON-объекта
        std::string request_body = request.dump();
        std::cout << "Request body: " << request_body << '\n';

        // Отправка POST-запроса на сервер
        cpr::Response r = cpr::Post(cpr::Url{"http://localhost:8080/jsonrpc"},
                                    cpr::Body{request_body},
                                    cpr::Header{{"Content-Type", "application/json"}});

        // Проверка ответа
        if (r.error) {
            std::cerr << "Error during request: " << r.error.message << '\n';
            continue;
        }

        // Обработка ответа сервера
        std::cout << "Status code: " << r.status_code << '\n';
        std::cout << "Response body: " << r.text << '\n';

        try {
            if (!r.text.empty()) {
                json response = json::parse(r.text);

                if (response.contains("error")) {
                    std::cout << "Error: " << response["error"]["message"] << '\n';
                } else {
                    std::cout << "Result: " << response["result"] << '\n';
                }
            } else {
                std::cerr << "Empty response body received from server.\n";
            }
        } catch (const json::parse_error& e) {
            std::cerr << "Parse error: " << e.what() << '\n';
        }
    }

    return 0;
}
