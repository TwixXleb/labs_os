#include <tools.h>
#include <iostream>
#include <string>
#include <thread>
#include <csignal>
#include <worker.h>
#include <sys/wait.h>

Node::Node(int id) : id(id), pid(-1) {
    socket = zmq::socket_t(globalContext, zmq::socket_type::req);
    socket.set(zmq::sockopt::rcvtimeo, 2000);  // Установим тайм-аут на 2 секунды
    socket.set(zmq::sockopt::linger, 0);
    sockId = id;

    std::string address = "tcp://127.0.0.1:" + std::to_string(5555 + sockId);
    int attempts = 0;
    while (true) {
        try {
            std::cout << "Attempting to connect to " << address << std::endl;  // Логирование попыток подключения
            socket.connect(address);
            break;
        } catch (zmq::error_t& e) {
            std::cerr << "Error connecting to " << address << ": " << e.what() << std::endl;  // Логирование ошибки подключения
            ++sockId;
            ++attempts;
            if (attempts >= 5) {  // Если не удается подключиться после 5 попыток, прерываем
                std::cerr << "Failed to connect after 5 attempts, giving up." << std::endl;
                throw std::runtime_error("Failed to connect to socket");
            }
        }
    }
}

extern std::shared_ptr<Node> root = nullptr;

// Таймерные функции
void StartTimer(std::shared_ptr<Node>& node) {
    if (node->start_time.has_value()) {
        return; // Если таймер уже запущен, не запускаем снова
    }
    node->start_time = std::chrono::steady_clock::now();
}

void StopTimer(std::shared_ptr<Node>& node) {
    if (!node->start_time.has_value()) {
        return; // Если таймер не был запущен, ничего не делаем
    }
    auto now = std::chrono::steady_clock::now();
    node->elapsed_time += std::chrono::duration_cast<std::chrono::milliseconds>(now - node->start_time.value());
    node->start_time.reset(); // Сбрасываем start_time, так как таймер остановлен
}

std::chrono::milliseconds GetElapsedTime(std::shared_ptr<Node>& node) {
    if (node->start_time.has_value()) {
        auto now = std::chrono::steady_clock::now();
        return node->elapsed_time + std::chrono::duration_cast<std::chrono::milliseconds>(now - node->start_time.value());
    }
    return node->elapsed_time; // Если таймер не был запущен, возвращаем только прошедшее время
}

// Узлы
std::shared_ptr<Node> FindNode(std::shared_ptr<Node> root, int id) {
    if (!root) return nullptr;
    if (root->id == id) return root;
    if (id < root->id) return FindNode(root->left, id);
    return FindNode(root->right, id);
}

bool InsertNode(std::shared_ptr<Node>& root, int id) {
    if (!root) {
        try {
            root = std::make_shared<Node>(id);
            pid_t pid = fork();
            if (pid == 0) {
                std::cout << "Starting worker for node " << id << "...\n";  // Логирование запуска процесса
                Worker(id, root->sockId);
                exit(0);
            } else if (pid < 0) {
                std::cerr << "Failed to fork for node " << id << ": " << strerror(errno) << std::endl;  // Логирование ошибки fork
                return false;
            }
            root->pid = pid;
            return true;
        } catch (zmq::error_t& e) {
            std::cerr << "Error while inserting node " << id << ": " << e.what() << std::endl;  // Логирование ошибки
            return false;
        }
    }
    if (id == root->id) return false;
    if (id < root->id) return InsertNode(root->left, id);
    return InsertNode(root->right, id);
}


void PingNodes(const std::shared_ptr<Node>& node, std::unordered_set<int>& unavailable_nodes) {
    if (!node) return;

    try {
        zmq::message_t message("ping");
        node->socket.send(message, zmq::send_flags::none);

        zmq::message_t reply;
        if (!node->socket.recv(reply, zmq::recv_flags::none) ||
            std::strcmp(reply.to_string().c_str(), "Ok") != 0) {
            unavailable_nodes.insert(node->id);
        }
    } catch (zmq::error_t&) {
        unavailable_nodes.insert(node->id);
    }

    PingNodes(node->left, unavailable_nodes);
    PingNodes(node->right, unavailable_nodes);
}

void TerminateNodes(const std::shared_ptr<Node>& node) {
    if (!node) return;

    if (waitpid(node->pid, nullptr, WNOHANG) != node->pid) {
        zmq::message_t message("exit");
        node->socket.send(message, zmq::send_flags::none);
    }

    try {
        node->socket.close();
    } catch (zmq::error_t& e) {
        std::cerr << "Error closing socket for node " << node->id << ": " << e.what() << "\n";
    }

    kill(node->pid, SIGKILL);
    waitpid(node->pid, nullptr, 0);

    TerminateNodes(node->left);
    TerminateNodes(node->right);
}
