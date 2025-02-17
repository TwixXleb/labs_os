#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <pthread.h>

const int MESSAGE_QUEUE_SIZE = 100;     // Размер очереди сообщений
const int MAX_MESSAGE_SIZE   = 1024;     // Максимальный размер данных сообщения
const int MAX_CLIENTS = 10;             // Для возможного расширения функциональности

// Возможные типы сообщений
enum MessageType {
    MSG_REGISTER,         // Регистрация: логин и пароль
    MSG_LOGIN,            // Вход в систему: логин и пароль
    MSG_CREATE_GAME,
    MSG_JOIN_GAME,
    MSG_PLACE_SHIPS,
    MSG_GAME_ACTION,
    MSG_REQUEST_BOARD,
    MSG_BOARD_STATE,
    MSG_GAME_UPDATE,
    MSG_INVITE,
    MSG_INVITE_RESPONSE,
    MSG_SHUTDOWN,
    MSG_ACK
};

// Структура сообщения
struct Message {
    int type;                 // Тип сообщения (MessageType)
    char sender[32];          // Логин отправителя
    char recipient[32];       // Логин получателя (либо "server" или "all")
    char data[MAX_MESSAGE_SIZE]; // Дополнительные данные (например, пароль, координаты, приглашение и т.д.)
};

// Очередь сообщений от клиентов к серверу (CS-queue)
struct CSQueue {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int message_count;
    int head;
    int tail;
    Message messages[MESSAGE_QUEUE_SIZE];
};

// Очередь сообщений от сервера к клиентам (SC-queue)
struct SCQueue {
    pthread_mutex_t mutex;
    int tail; // индекс для следующего сообщения
    Message messages[MESSAGE_QUEUE_SIZE];
};

// Общая структура памяти
struct SharedMemory {
    CSQueue cs_queue;
    SCQueue sc_queue;
};

#endif // SHARED_MEMORY_H
