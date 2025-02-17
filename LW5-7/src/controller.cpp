#include <controller.h>
#include <tools.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <future>
#include <vector>

std::string Exec(const std::shared_ptr<Node>& node, const std::string& command) {
    try {
        zmq::message_t message(command);
        node->socket.send(message, zmq::send_flags::none);

        zmq::message_t reply;
        if (node->socket.recv(reply, zmq::recv_flags::none)) {
            return reply.to_string();
        }
    } catch (zmq::error_t&) {
        return "Error: Node is unavailable";
    }
    return "Error: Node is unavailable";
}

void Controller(std::istream& stream, bool test) {
    while (true) {
        if (!test) {
            std::cout << "command> ";
            std::cout.flush();
        }

        std::string command;
        if (std::getline(stream, command)) {
            std::istringstream iss(command);
            std::string cmdType;
            iss >> cmdType;

            if (cmdType == "create") {
                int id;
                iss >> id;

                if (!InsertNode(root, id)) {
                    std::cout << "Error: Already exists\n";
                } else {
                    std::cout << "Ok\n";
                }
            } else if (cmdType == "exec") {
                int id;
                std::string subcommand;
                iss >> id >> subcommand;

                auto node = FindNode(root, id);
                if (!node) {
                    std::cout << "Error: Not found\n";
                    continue;
                }

                if (subcommand == "start") {
                    StartTimer(node);
                    std::cout << "Ok:" << id << "\n";
                } else if (subcommand == "stop") {
                    StopTimer(node);
                    std::cout << "Ok:" << id << "\n";
                } else if (subcommand == "time") {
                    auto elapsed = GetElapsedTime(node);
                    std::cout << "Ok:" << id << ": " << elapsed.count() << "\n";
                } else {
                    std::cout << "Error: Unknown subcommand\n";
                }
            } else if (cmdType == "ping") {
                int id;
                iss >> id;

                auto node = FindNode(root, id);
                if (!node) {
                    std::cout << "Error: Not found\n";
                    continue;
                }

                try {
                    zmq::message_t message("ping", 4);  // Используем новый конструктор
                    std::cout << "Pinging node " << id << "...\n";  // Логирование пинга
                    node->socket.send(message, zmq::send_flags::none);

                    zmq::message_t reply;
                    zmq::recv_result_t result = node->socket.recv(reply, zmq::recv_flags::none);
                    bool received = result.has_value();  // Проверяем, был ли получен ответ
                    if (received && std::strcmp(reply.to_string().c_str(), "Ok") == 0) {
                        std::cout << "Ok: 1\n";  // Ответ от узла
                    } else {
                        std::cout << "Error: Unexpected reply or no response\n";  // Неправильный ответ или отсутствие ответа
                        std::cout << "Ok: 0\n";
                    }
                } catch (zmq::error_t& e) {
                    std::cerr << "Ping failed for node " << id << ": " << e.what() << std::endl;
                    std::cout << "Ok: 0\n";  // Ошибка при пинге
                }
            } else if (cmdType == "exit") {
                TerminateNodes(root);
                globalContext.close();
                break;
            } else {
                std::cout << "Error: Unknown command\n";
            }
        }
    }
}
