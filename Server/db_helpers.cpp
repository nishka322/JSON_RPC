#include "db_helpers.h"

// Функция для получения данных о собаке по ID
nlohmann::json get_dog_by_id(pqxx::connection& conn, int id) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec("SELECT * FROM dogs WHERE id = " + txn.quote(id));

    nlohmann::json response;
    if (!res.empty()) {
        auto row = res[0];
        response = {
                {"id", row["id"].as<int>()},
                {"name", row["name"].as<std::string>()},
                {"breed", row["breed"].as<std::string>()},
                {"age", row["age"].as<int>()}
        };
    }
    return response;
}

// Функция для получения всех собак по породе
nlohmann::json get_dogs_by_breed(pqxx::connection& conn, const std::string& breed) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec("SELECT * FROM dogs WHERE breed = " + txn.quote(breed));

    nlohmann::json result = nlohmann::json::array();
    for (const auto& row : res) {
        result.push_back({
                                 {"id", row["id"].as<int>()},
                                 {"name", row["name"].as<std::string>()},
                                 {"breed", row["breed"].as<std::string>()},
                                 {"age", row["age"].as<int>()}
                         });
    }
    return result;
}

// Функция для получения всех собак по возрасту
nlohmann::json get_dogs_by_age(pqxx::connection& conn, int age) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec("SELECT * FROM dogs WHERE age = " + txn.quote(age));

    nlohmann::json result = nlohmann::json::array();
    for (const auto& row : res) {
        result.push_back({
                                 {"id", row["id"].as<int>()},
                                 {"name", row["name"].as<std::string>()},
                                 {"breed", row["breed"].as<std::string>()},
                                 {"age", row["age"].as<int>()}
                         });
    }
    return result;
}

// Функция для получения данных о собаке по имени
nlohmann::json get_dog_by_name(pqxx::connection& conn, const std::string& name) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec("SELECT * FROM dogs WHERE name = " + txn.quote(name));

    nlohmann::json response;
    if (!res.empty()) {
        auto row = res[0];
        response = {
                {"id", row["id"].as<int>()},
                {"name", row["name"].as<std::string>()},
                {"breed", row["breed"].as<std::string>()},
                {"age", row["age"].as<int>()}
        };
    }
    return response;
}

bool update_dog(pqxx::connection& conn, int id, const std::string& name, const std::string& breed, int age) {
    try {
        pqxx::work txn(conn);

        // Проверяем, существует ли запись с таким id
        pqxx::result res = txn.exec("SELECT id FROM dogs WHERE id = " + txn.quote(id));
        if (res.empty()) {
            return false; // Собака с таким id не найдена
        }

        // Выполняем запрос на обновление данных
        txn.exec0("UPDATE dogs SET name = " + txn.quote(name) +
                  ", breed = " + txn.quote(breed) +
                  ", age = " + txn.quote(age) +
                  " WHERE id = " + txn.quote(id));
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error updating dog: " << e.what() << std::endl;
        return false;
    }
}