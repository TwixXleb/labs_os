#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <mutex>
#include <sys/select.h>
#include <limits>
#include <string>
#include "../include/shared_memory.h"

// Имя разделяемой памяти
const char* SHM_NAME = "/battle_shm";

// Глобальные переменные для логина (защищены мьютексом)
std::string gAttemptedLogin = "";
std::string gCurrentLogin = "";
std::mutex gLoginMutex;

// Флаги аутентификации и состояния игры
std::atomic<bool> loggedIn(false);
std::atomic<bool> gameReadyForPlacement(false); // устанавливается, когда сервер подтверждает завершение расстановки кораблей
std::atomic<bool> gameStarted(false);           // устанавливается, когда начинается игра
std::atomic<bool> gameOver(false);              // новый флаг: игра завершена (победа одного из игроков)

// Почтовый ящик для приглашений и его мьютекс
std::vector<Message> mailbox;
std::mutex mailboxMutex;

void sendCSMessage(SharedMemory* shm, const Message &msg) {
    pthread_mutex_lock(&shm->cs_queue.mutex);
    if (shm->cs_queue.message_count < MESSAGE_QUEUE_SIZE) {
        shm->cs_queue.messages[shm->cs_queue.tail] = msg;
        shm->cs_queue.tail = (shm->cs_queue.tail + 1) % MESSAGE_QUEUE_SIZE;
        shm->cs_queue.message_count++;
        pthread_cond_signal(&shm->cs_queue.cond);
    } else {
        std::cerr << "Очередь сообщений переполнена!" << std::endl;
    }
    pthread_mutex_unlock(&shm->cs_queue.mutex);
}

void scListener(SharedMemory* shm) {
    int lastRead = 0;
    while (true) {
        pthread_mutex_lock(&shm->sc_queue.mutex);
        int currentTail = shm->sc_queue.tail;
        while (lastRead != currentTail) {
            Message msg = shm->sc_queue.messages[lastRead];
            lastRead = (lastRead + 1) % MESSAGE_QUEUE_SIZE;
            std::string attempted;
            {
                std::lock_guard<std::mutex> lock(gLoginMutex);
                attempted = gCurrentLogin.empty() ? gAttemptedLogin : gCurrentLogin;
            }
            if (!attempted.empty() &&
                (strcmp(msg.recipient, attempted.c_str()) == 0 ||
                 strcmp(msg.recipient, "all") == 0))
            {
                if (msg.type == MSG_BOARD_STATE) {
                    std::cout << "\n[ДОСКА от " << msg.sender << "]:\n" << msg.data << std::endl;
                } else {
                    std::cout << "\n[Сообщение от " << msg.sender << "]: " << msg.data << std::endl;
                }
                std::string dataStr(msg.data);
                if (msg.type == MSG_GAME_UPDATE &&
                    (dataStr.find("Вы успешно вошли в систему") != std::string::npos ||
                     dataStr.find("Регистрация прошла успешно") != std::string::npos))
                {
                    loggedIn = true;
                    {
                        std::lock_guard<std::mutex> lock(gLoginMutex);
                        gCurrentLogin = attempted;
                    }
                }
                if (msg.type == MSG_GAME_UPDATE &&
                    dataStr.find("Начните расстановку кораблей") != std::string::npos)
                {
                    gameReadyForPlacement = true;
                }
                if (msg.type == MSG_GAME_UPDATE &&
                    dataStr.find("Игра начинается") != std::string::npos)
                {
                    gameStarted = true;
                }
                // Если сообщение содержит информацию о завершении игры – устанавливаем флаг gameOver
                if (msg.type == MSG_GAME_UPDATE &&
                    dataStr.find("Игра окончена") != std::string::npos)
                {
                    gameOver = true;
                }
                if (msg.type == MSG_INVITE) {
                    std::lock_guard<std::mutex> lock(mailboxMutex);
                    mailbox.push_back(msg);
                    std::cout << "[MAILBOX] Новое приглашение от " << msg.sender << std::endl;
                }
            }
        }
        pthread_mutex_unlock(&shm->sc_queue.mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// Фаза расстановки кораблей (пример без использования select, меню выводится на каждой итерации)
void runPlacementPhase(SharedMemory* shm) {
    std::cout << "\nПереход в режим расстановки кораблей." << std::endl;
    while (!gameStarted.load() && !gameOver.load()) {
        std::cout << "\n--- Фаза расстановки кораблей ---" << std::endl;
        std::cout << "Введите команду в формате:" << std::endl;
        std::cout << "  type start_row start_col end_row end_col" << std::endl;
        std::cout << "Например, для 4-палубника: 4 0 0 0 3" << std::endl;
        std::cout << "2. Просмотреть свою доску" << std::endl;
        std::cout << "Ваш выбор: ";

        std::string input;
        if (!std::getline(std::cin, input)) {
            break;
        }
        // Если сервер сообщил о начале игры или игре окончено, выходим
        if (gameStarted.load() || gameOver.load())
            break;

        if (input == "2") {
            Message bMsg;
            bMsg.type = MSG_REQUEST_BOARD;
            strncpy(bMsg.sender, gCurrentLogin.c_str(), sizeof(bMsg.sender) - 1);
            strncpy(bMsg.recipient, "server", sizeof(bMsg.recipient) - 1);
            strcpy(bMsg.data, "");
            sendCSMessage(shm, bMsg);
        } else {
            Message pMsg;
            pMsg.type = MSG_PLACE_SHIPS;
            strncpy(pMsg.sender, gCurrentLogin.c_str(), sizeof(pMsg.sender) - 1);
            strncpy(pMsg.recipient, "server", sizeof(pMsg.recipient) - 1);
            strncpy(pMsg.data, input.c_str(), sizeof(pMsg.data) - 1);
            sendCSMessage(shm, pMsg);
        }
    }
}

// Фаза ходов
void runMovePhase(SharedMemory* shm) {
    std::cout << "\nПереход в режим ходов." << std::endl;
    while (true) {
        // Если игра окончена, выходим из цикла ходов и возвращаемся в главное меню
        if (gameOver.load()) {
            std::cout << "\nИгра окончена, возвращаемся в главное меню." << std::endl;
            break;
        }
        std::cout << "\n--- Игровое меню ---" << std::endl;
        std::cout << "1. Сделать ход" << std::endl;
        std::cout << "2. Показать доску" << std::endl;
        std::cout << "0. Выйти из игры (вернуться в стартовое меню)" << std::endl;
        std::cout << "Ваш выбор: ";
        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Некорректный ввод. Попробуйте снова." << std::endl;
            continue;
        }
        std::cin.ignore(10000, '\n');
        if (choice == 1) {
            std::string coords;
            std::cout << "Введите координаты для хода (row col): ";
            std::getline(std::cin, coords);
            Message mMsg;
            mMsg.type = MSG_GAME_ACTION;
            strncpy(mMsg.sender, gCurrentLogin.c_str(), sizeof(mMsg.sender) - 1);
            strncpy(mMsg.recipient, "server", sizeof(mMsg.recipient) - 1);
            strncpy(mMsg.data, coords.c_str(), sizeof(mMsg.data) - 1);
            sendCSMessage(shm, mMsg);
        } else if (choice == 2) {
            Message bMsg;
            bMsg.type = MSG_REQUEST_BOARD;
            strncpy(bMsg.sender, gCurrentLogin.c_str(), sizeof(bMsg.sender) - 1);
            strncpy(bMsg.recipient, "server", sizeof(bMsg.recipient) - 1);
            strcpy(bMsg.data, "");
            sendCSMessage(shm, bMsg);
        } else if (choice == 0) {
            break;
        } else {
            std::cout << "Неверный выбор." << std::endl;
        }
    }
}

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    SharedMemory* shm_ptr = (SharedMemory*) mmap(nullptr, sizeof(SharedMemory),
                                                 PROT_READ | PROT_WRITE, MAP_SHARED,
                                                 shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    std::thread listener(scListener, shm_ptr);
    listener.detach();

    // Цикл аутентификации
    bool exitAuth = false;
    while (!loggedIn && !exitAuth) {
        std::cout << "\n--- Аутентификация ---" << std::endl;
        std::cout << "Выберите действие:" << std::endl;
        std::cout << "1. Войти" << std::endl;
        std::cout << "2. Зарегистрироваться" << std::endl;
        std::cout << "0. Выход" << std::endl;
        std::cout << "Ваш выбор: ";
        int authChoice = 0;
        if (!(std::cin >> authChoice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Некорректный ввод. Попробуйте снова." << std::endl;
            continue;
        }
        if (authChoice == 0) {
            exitAuth = true;
            break;
        }
        std::string login, password;
        if (authChoice == 1) {
            std::cout << "Введите логин: ";
            std::cin >> login;
            std::cout << "Введите пароль: ";
            std::cin >> password;
            {
                std::lock_guard<std::mutex> lock(gLoginMutex);
                gAttemptedLogin = login;
            }
            Message msg;
            msg.type = MSG_LOGIN;
            strncpy(msg.sender, login.c_str(), sizeof(msg.sender)-1);
            strncpy(msg.recipient, "server", sizeof(msg.recipient)-1);
            strncpy(msg.data, password.c_str(), sizeof(msg.data)-1);
            sendCSMessage(shm_ptr, msg);
        } else if (authChoice == 2) {
            std::cout << "Введите логин: ";
            std::cin >> login;
            std::cout << "Введите пароль: ";
            std::cin >> password;
            {
                std::lock_guard<std::mutex> lock(gLoginMutex);
                gAttemptedLogin = login;
            }
            Message msg;
            msg.type = MSG_REGISTER;
            strncpy(msg.sender, login.c_str(), sizeof(msg.sender)-1);
            strncpy(msg.recipient, "server", sizeof(msg.recipient)-1);
            strncpy(msg.data, password.c_str(), sizeof(msg.data)-1);
            sendCSMessage(shm_ptr, msg);
        } else {
            std::cout << "Неверный выбор." << std::endl;
        }
        std::cin.ignore(10000, '\n');
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (!loggedIn) {
            std::cout << "Вход не удался. Попробуйте ещё раз или введите 0 для выхода." << std::endl;
        }
    }
    if (!loggedIn) {
        std::cout << "Аутентификация не завершена. Выход из программы." << std::endl;
        munmap(shm_ptr, sizeof(SharedMemory));
        close(shm_fd);
        return 0;
    }
    {
        std::lock_guard<std::mutex> lock(gLoginMutex);
        gCurrentLogin = gAttemptedLogin;
    }
    std::cout << "\nУспешный вход! Ваш логин: " << gCurrentLogin << std::endl;

    // Стартовое меню
    bool exitProgram = false;
    while (!exitProgram) {
        // Сброс флагов перед новой игрой
        gameStarted = false;
        gameOver = false;
        gameReadyForPlacement = false;

        std::cout << "\n--- Стартовое меню ---" << std::endl;
        std::cout << "1. Начать игру (создать игру)" << std::endl;
        std::cout << "2. Почтовый ящик (просмотр приглашений)" << std::endl;
        std::cout << "0. Выход" << std::endl;
        std::cout << "Ваш выбор: ";
        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Некорректный ввод. Попробуйте снова." << std::endl;
            continue;
        }
        std::cin.ignore(10000, '\n');
        switch (choice) {
            case 1: {
                Message msg;
                msg.type = MSG_CREATE_GAME;
                strncpy(msg.sender, gCurrentLogin.c_str(), sizeof(msg.sender)-1);
                strncpy(msg.recipient, "server", sizeof(msg.recipient)-1);
                strcpy(msg.data, "Запрос на создание игры");
                sendCSMessage(shm_ptr, msg);
                std::cout << "Игра создана." << std::endl;
                std::string target;
                std::cout << "Введите логин игрока для приглашения: ";
                std::getline(std::cin, target);
                Message inviteMsg;
                inviteMsg.type = MSG_INVITE;
                strncpy(inviteMsg.sender, gCurrentLogin.c_str(), sizeof(inviteMsg.sender)-1);
                strncpy(inviteMsg.recipient, target.c_str(), sizeof(inviteMsg.recipient)-1);
                strcpy(inviteMsg.data, "Приглашение на игру в Морской бой");
                sendCSMessage(shm_ptr, inviteMsg);
                std::cout << "Приглашение отправлено игроку " << target << ". Ожидайте присоединения." << std::endl;

                // Цикл ожидания второго игрока
                while (!gameReadyForPlacement.load() && !gameOver.load()) {
                    static int dots = 0;
                    dots = (dots + 1) % 4;
                    std::cout << "\rОжидание начала игры" << std::string(dots, '.') << "   " << std::flush;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                std::cout << std::endl;
                gameReadyForPlacement = false;
                runPlacementPhase(shm_ptr);
                // Перед запуском фазы ходов очищаем входной поток
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                runMovePhase(shm_ptr);
                break;
            }
            case 2: {
                std::lock_guard<std::mutex> lock(mailboxMutex);
                if (mailbox.empty()) {
                    std::cout << "\nВаш почтовый ящик пуст." << std::endl;
                } else {
                    std::cout << "\n--- Почтовый ящик ---" << std::endl;
                    for (size_t i = 0; i < mailbox.size(); i++) {
                        std::cout << i+1 << ". Приглашение от " << mailbox[i].sender
                                  << ": " << mailbox[i].data << std::endl;
                    }
                    std::cout << "Введите номер приглашения для принятия, 0 для очистки ящика, или -1 для выхода: ";
                    int mailChoice = 0;
                    if (!(std::cin >> mailChoice)) {
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        std::cout << "Некорректный ввод." << std::endl;
                        break;
                    }
                    std::cin.ignore(10000, '\n');
                    if (mailChoice > 0 && (size_t)mailChoice <= mailbox.size()) {
                        Message joinMsg;
                        joinMsg.type = MSG_JOIN_GAME;
                        strncpy(joinMsg.sender, gCurrentLogin.c_str(), sizeof(joinMsg.sender)-1);
                        strncpy(joinMsg.recipient, "server", sizeof(joinMsg.recipient)-1);
                        strcpy(joinMsg.data, "Принял приглашение");
                        sendCSMessage(shm_ptr, joinMsg);
                        mailbox.erase(mailbox.begin() + mailChoice - 1);
                        gameReadyForPlacement = false;
                        runPlacementPhase(shm_ptr);
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        runMovePhase(shm_ptr);
                    } else if (mailChoice == 0) {
                        mailbox.clear();
                        std::cout << "Почтовый ящик очищен." << std::endl;
                    } else if (mailChoice == -1) {
                        // Выходим из почтового ящика
                    } else {
                        std::cout << "Неверный выбор." << std::endl;
                    }
                }
                break;
            }
            case 0:
                exitProgram = true;
                break;
            default:
                std::cout << "Неверный выбор." << std::endl;
                break;
        }
    }
    munmap(shm_ptr, sizeof(SharedMemory));
    close(shm_fd);
    return 0;
}
