#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <queue>
#include <algorithm>
#include "../include/shared_memory.h"

// Имя разделяемой памяти и файл базы данных
const char* SHM_NAME = "/battle_shm";
const char* PLAYERS_DB_FILE = "players.db";

// ------------------------------
// Глобальные переменные
// ------------------------------
struct Game {
    bool active;            // Игра создана
    bool started;           // Игра началась (оба игрока готовы)
    char player1[32];       // Логин первого игрока (создателя игры)
    char player2[32];       // Логин второго игрока (присоединившегося)
    int turn;               // Чей ход: 1 или 2
    char board1[10][10];    // Доска первого игрока
    char board2[10][10];    // Доска второго игрока
    bool player1Ready;      // Игрок 1 завершил расстановку кораблей
    bool player2Ready;      // Игрок 2 завершил расстановку кораблей
    int player1Ships[5];    // Счётчики кораблей для игрока 1 (индексы 1..4)
    int player2Ships[5];    // Для игрока 2
};

Game currentGame = { false, false, "", "", 0, {}, {}, false, false, {0,0,0,0,0}, {0,0,0,0,0} };
std::map<std::string, std::string> playersDB;  // база: логин -> пароль
std::set<std::string> onlinePlayers;           // множество онлайн игроков

// ------------------------------
// Прототипы функций
// ------------------------------
void sendSCMessage(SharedMemory* shm, const Message &msg);
void sendGameUpdate(SharedMemory* shm, const char* recipient, const std::string &update);
void initializeEmptyBoard(char board[10][10]);
std::string boardToString(char board[10][10]);
std::string boardToStringPublic(char board[10][10]);
void sendBoardState(SharedMemory* shm, const char* player);
bool isAllShipsSunk(char board[10][10]);
void processGameAction(SharedMemory* shm, const Message &msg);
void processPlaceShips(SharedMemory* shm, const Message &msg);
void processInvite(SharedMemory* shm, const Message &msg);
void loadPlayersDatabase();
void addPlayerToDatabase(const std::string& login, const std::string& password);
void processRegister(SharedMemory* shm, const Message &msg);
void processLogin(SharedMemory* shm, const Message &msg);
void processCreateGame(SharedMemory* shm, const Message &msg);
void processJoinGame(SharedMemory* shm, const Message &msg);

// Дополнительные функции для кораблей
bool isShipSunkAndCollect(int i, int j, char board[10][10], std::vector<std::pair<int,int>> &shipCells) {
    bool sunk = true;
    bool visited[10][10] = {false};
    std::queue<std::pair<int,int>> q;
    q.push({i,j});
    visited[i][j] = true;
    while(!q.empty()){
        auto [r, c] = q.front();
        q.pop();
        shipCells.push_back({r,c});
        int dr[4] = {-1, 1, 0, 0};
        int dc[4] = {0, 0, -1, 1};
        for (int k = 0; k < 4; k++){
            int nr = r + dr[k], nc = c + dc[k];
            if(nr >= 0 && nr < 10 && nc >= 0 && nc < 10){
                if(!visited[nr][nc] && (board[nr][nc]=='#' || board[nr][nc]=='X' || board[nr][nc]=='■')){
                    visited[nr][nc] = true;
                    q.push({nr, nc});
                }
            }
        }
    }
    for (auto &p : shipCells){
        if(board[p.first][p.second]=='#'){
            sunk = false;
            break;
        }
    }
    return sunk;
}

void markSurroundingCells(const std::vector<std::pair<int,int>> &shipCells, char board[10][10]) {
    int dr[8] = {-1,-1,-1,0,0,1,1,1};
    int dc[8] = {-1,0,1,-1,1,-1,0,1};
    for (auto &p : shipCells){
        int r = p.first, c = p.second;
        for (int k = 0; k < 8; k++){
            int nr = r + dr[k], nc = c + dc[k];
            if(nr >= 0 && nr < 10 && nc >= 0 && nc < 10){
                if(board[nr][nc]=='.')
                    board[nr][nc] = '0';
            }
        }
    }
}

bool isPlacementValid(int sr, int sc, int er, int ec, char board[10][10]) {
    int rmin = std::max(0, std::min(sr, er) - 1);
    int rmax = std::min(9, std::max(sr, er) + 1);
    int cmin = std::max(0, std::min(sc, ec) - 1);
    int cmax = std::min(9, std::max(sc, ec) + 1);
    for (int i = rmin; i <= rmax; i++){
        for (int j = cmin; j <= cmax; j++){
            if(board[i][j]=='#' || board[i][j]=='X' || board[i][j]=='■')
                return false;
        }
    }
    return true;
}

// ------------------------------
// Обработка сообщений от клиентов
// ------------------------------
void sendSCMessage(SharedMemory* shm, const Message &msg) {
    pthread_mutex_lock(&shm->sc_queue.mutex);
    int index = shm->sc_queue.tail;
    shm->sc_queue.messages[index] = msg;
    shm->sc_queue.tail = (index + 1) % MESSAGE_QUEUE_SIZE;
    pthread_mutex_unlock(&shm->sc_queue.mutex);
}

void sendGameUpdate(SharedMemory* shm, const char* recipient, const std::string &update) {
    Message msg;
    msg.type = MSG_GAME_UPDATE;
    strncpy(msg.sender, "server", sizeof(msg.sender)-1);
    strncpy(msg.recipient, recipient, sizeof(msg.recipient)-1);
    strncpy(msg.data, update.c_str(), sizeof(msg.data)-1);
    sendSCMessage(shm, msg);
    std::cout << "[LOG] Отправлено обновление для " << recipient << ": " << update << std::endl;
}

void initializeEmptyBoard(char board[10][10]) {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            board[i][j] = '.';
}

std::string boardToString(char board[10][10]) {
    std::string result;
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            result.push_back(board[i][j]);
            result.push_back(' ');
        }
        result.push_back('\n');
    }
    return result;
}

std::string boardToStringPublic(char board[10][10]) {
    std::string result;
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            char cell = board[i][j];
            if (cell == '#')
                cell = '.';
            result.push_back(cell);
            result.push_back(' ');
        }
        result.push_back('\n');
    }
    return result;
}

void sendBoardState(SharedMemory* shm, const char* player) {
    Message msg;
    msg.type = MSG_BOARD_STATE;
    strncpy(msg.sender, "server", sizeof(msg.sender)-1);
    strncpy(msg.recipient, player, sizeof(msg.recipient)-1);
    std::string boardInfo;
    if (!currentGame.started) {
        if (strcmp(player, currentGame.player1) == 0) {
            boardInfo = "Ваша доска:\n" + boardToString(currentGame.board1);
        } else if (strcmp(player, currentGame.player2) == 0) {
            boardInfo = "Ваша доска:\n" + boardToString(currentGame.board2);
        } else {
            boardInfo = "Вы не участвуете в игре.";
        }
    } else {
        if (strcmp(player, currentGame.player1) == 0) {
            boardInfo = "Ваша доска:\n" + boardToString(currentGame.board1) +
                        "\nДоска противника:\n" + boardToStringPublic(currentGame.board2);
        } else if (strcmp(player, currentGame.player2) == 0) {
            boardInfo = "Ваша доска:\n" + boardToString(currentGame.board2) +
                        "\nДоска противника:\n" + boardToStringPublic(currentGame.board1);
        } else {
            boardInfo = "Вы не участвуете в игре.";
        }
    }
    strncpy(msg.data, boardInfo.c_str(), sizeof(msg.data)-1);
    sendSCMessage(shm, msg);
    std::cout << "[LOG] Отправлено состояние доски для " << player << std::endl;
}

bool isAllShipsSunk(char board[10][10]) {
    for (int i = 0; i < 10; i++){
        for (int j = 0; j < 10; j++){
            if (board[i][j] == '#')
                return false;
        }
    }
    return true;
}

void processGameAction(SharedMemory* shm, const Message &msg) {
    int row, col;
    if (sscanf(msg.data, "%d %d", &row, &col) != 2) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        snprintf(reply.data, sizeof(reply.data), "Неверный формат координат. Используйте: row col");
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Неверный формат координат от " << msg.sender << std::endl;
        return;
    }
    if (row < 0 || row >= 10 || col < 0 || col >= 10) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        snprintf(reply.data, sizeof(reply.data), "Координаты вне диапазона (0-9).");
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Некорректные координаты от " << msg.sender << std::endl;
        return;
    }
    if (!(currentGame.player1Ready && currentGame.player2Ready)) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Ожидается завершение расстановки кораблей обоими игроками.");
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Попытка хода до завершения расстановки от " << msg.sender << std::endl;
        return;
    }
    bool validTurn = false;
    char* opponentBoard = nullptr;
    if (currentGame.turn == 1 && strcmp(msg.sender, currentGame.player1) == 0) {
        validTurn = true;
        opponentBoard = &currentGame.board2[0][0];
    } else if (currentGame.turn == 2 && strcmp(msg.sender, currentGame.player2) == 0) {
        validTurn = true;
        opponentBoard = &currentGame.board1[0][0];
    }
    if (!validTurn) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Не ваш ход.");
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Выстрел вне очереди от " << msg.sender << std::endl;
        return;
    }
    char (*board)[10] = (char (*)[10])opponentBoard;
    char cell = board[row][col];
    std::string result;
    if (cell == '#') {
        board[row][col] = 'X';
        std::vector<std::pair<int,int>> shipCells;
        bool sunk = isShipSunkAndCollect(row, col, board, shipCells);
        if (sunk) {
            result = "Корабль потоплен! Попадание!";
            markSurroundingCells(shipCells, board);
        } else {
            result = "Попадание!";
        }
        // При попадании игрок продолжает ход – очередь не меняется.
    } else if (cell == '.') {
        board[row][col] = '0';
        result = "Промах!";
        currentGame.turn = (currentGame.turn == 1) ? 2 : 1;
    } else {
        result = "Эта клетка уже выбрана.";
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strncpy(reply.data, result.c_str(), sizeof(reply.data)-1);
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Выстрел в занятую клетку от " << msg.sender << std::endl;
        return;
    }
    sendBoardState(shm, currentGame.player1);
    sendBoardState(shm, currentGame.player2);
    char updateMsg[1024];
    bool win = false;
    if (strcmp(msg.sender, currentGame.player1) == 0)
        win = isAllShipsSunk(currentGame.board2);
    else
        win = isAllShipsSunk(currentGame.board1);
    if (win) {
        snprintf(updateMsg, sizeof(updateMsg),
                 "Игрок %s сделал ход (%d, %d): %s\nИгра окончена! Победитель: %s",
                 msg.sender, row, col, result.c_str(), msg.sender);
        sendGameUpdate(shm, currentGame.player1, updateMsg);
        sendGameUpdate(shm, currentGame.player2, updateMsg);
        std::cout << "[LOG] Игра окончена. Победитель: " << msg.sender << std::endl;
        currentGame.active = false;
        currentGame.started = false;
    } else {
        snprintf(updateMsg, sizeof(updateMsg),
                 "Игрок %s сделал ход (%d, %d): %s\nСледующий ход: %s",
                 msg.sender, row, col, result.c_str(),
                 (currentGame.turn == 1 ? currentGame.player1 : currentGame.player2));
        sendGameUpdate(shm, currentGame.player1, updateMsg);
        sendGameUpdate(shm, currentGame.player2, updateMsg);
        std::cout << "[LOG] Ход выполнен. Следующий ход: "
                  << (currentGame.turn == 1 ? currentGame.player1 : currentGame.player2)
                  << std::endl;
    }
}

void processPlaceShips(SharedMemory* shm, const Message &msg) {
    // Убираем ветку явного завершения ("0") – завершаем фазу автоматически.
    int allowed[5] = {0, 4, 3, 2, 1};  // Для 1-палубных: 4, 2-палубных: 3, 3-палубных: 2, 4-палубных: 1
    int shipType, sr, sc, er, ec;
    if (sscanf(msg.data, "%d %d %d %d %d", &shipType, &sr, &sc, &er, &ec) != 5) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Неверный формат. Используйте: type start_row start_col end_row end_col");
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Неверный формат расстановки корабля от " << msg.sender << std::endl;
        return;
    }
    if (shipType < 1 || shipType > 4) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Тип корабля должен быть от 1 до 4.");
        sendSCMessage(shm, reply);
        return;
    }
    bool horizontal = (sr == er);
    bool vertical = (sc == ec);
    if (!horizontal && !vertical) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Корабль должен располагаться горизонтально или вертикально.");
        sendSCMessage(shm, reply);
        return;
    }
    int length = horizontal ? (ec - sc + 1) : (er - sr + 1);
    if (length != shipType) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Длина корабля (%d) не соответствует выбранному типу (%d).", length, shipType);
        strncpy(reply.data, buffer, sizeof(reply.data)-1);
        sendSCMessage(shm, reply);
        return;
    }
    if (sr < 0 || sr >= 10 || er < 0 || er >= 10 || sc < 0 || sc >= 10 || ec < 0 || ec >= 10) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Координаты вне диапазона (0-9).");
        sendSCMessage(shm, reply);
        return;
    }
    char* boardPtr = nullptr;
    int* shipCount = nullptr;
    if (strcmp(msg.sender, currentGame.player1) == 0) {
        boardPtr = &currentGame.board1[0][0];
        shipCount = currentGame.player1Ships;
    } else if (strcmp(msg.sender, currentGame.player2) == 0) {
        boardPtr = &currentGame.board2[0][0];
        shipCount = currentGame.player2Ships;
    } else {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Вы не участвуете в игре.");
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Попытка расстановки кораблей от неизвестного игрока: " << msg.sender << std::endl;
        return;
    }
    char (*board)[10] = (char (*)[10])boardPtr;
    if (!isPlacementValid(sr, sc, er, ec, board)) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Нарушено правило: корабли не должны касаться друг друга, даже по диагонали.");
        sendSCMessage(shm, reply);
        return;
    }
    if (horizontal) {
        for (int j = sc; j <= ec; j++) {
            if (board[sr][j] != '.') {
                Message reply;
                reply.type = MSG_GAME_UPDATE;
                strncpy(reply.sender, "server", sizeof(reply.sender)-1);
                strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
                char buffer[128];
                snprintf(buffer, sizeof(buffer), "Клетка (%d, %d) уже занята.", sr, j);
                strncpy(reply.data, buffer, sizeof(reply.data)-1);
                sendSCMessage(shm, reply);
                return;
            }
        }
    } else {
        for (int i = sr; i <= er; i++) {
            if (board[i][sc] != '.') {
                Message reply;
                reply.type = MSG_GAME_UPDATE;
                strncpy(reply.sender, "server", sizeof(reply.sender)-1);
                strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
                char buffer[128];
                snprintf(buffer, sizeof(buffer), "Клетка (%d, %d) уже занята.", i, sc);
                strncpy(reply.data, buffer, sizeof(reply.data)-1);
                sendSCMessage(shm, reply);
                return;
            }
        }
    }
    // Проверяем, что уже не превышено допустимое количество кораблей данного типа
    if (shipCount[shipType] >= allowed[shipType]) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Все корабли типа %d уже расставлены.", shipType);
        strncpy(reply.data, buffer, sizeof(reply.data)-1);
        sendSCMessage(shm, reply);
        return;
    }
    if (horizontal) {
        for (int j = sc; j <= ec; j++) {
            board[sr][j] = '#';
        }
    } else {
        for (int i = sr; i <= er; i++) {
            board[i][sc] = '#';
        }
    }
    shipCount[shipType]++;
    Message reply;
    reply.type = MSG_GAME_UPDATE;
    strncpy(reply.sender, "server", sizeof(reply.sender)-1);
    strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Корабль типа %d размещён.", shipType);
    strncpy(reply.data, buffer, sizeof(reply.data)-1);
    sendSCMessage(shm, reply);
    std::cout << "[LOG] Игрок " << msg.sender << " разместил корабль типа " << shipType
              << " от (" << sr << "," << sc << ") до (" << er << "," << ec << ")" << std::endl;

    int totalShips = shipCount[1] + shipCount[2] + shipCount[3] + shipCount[4];
    if (totalShips == 10) {
        if (strcmp(msg.sender, currentGame.player1) == 0)
            currentGame.player1Ready = true;
        else
            currentGame.player2Ready = true;
        Message readyMsg;
        readyMsg.type = MSG_GAME_UPDATE;
        strncpy(readyMsg.sender, "server", sizeof(readyMsg.sender)-1);
        strncpy(readyMsg.recipient, msg.sender, sizeof(readyMsg.recipient)-1);
        strcpy(readyMsg.data, "Вы завершили расстановку кораблей. Ожидайте начала игры.");
        sendSCMessage(shm, readyMsg);
        if (currentGame.player1Ready && currentGame.player2Ready) {
            currentGame.started = true;
            currentGame.turn = 1;
            char updateMsg[256];
            snprintf(updateMsg, sizeof(updateMsg), "Оба игрока расставили корабли. Игра начинается. Ход первого игрока: %s", currentGame.player1);
            sendGameUpdate(shm, currentGame.player1, updateMsg);
            sendGameUpdate(shm, currentGame.player2, updateMsg);
            std::cout << "[LOG] Игра начинается. Ход первого игрока: " << currentGame.player1 << std::endl;
        }
    }
}

void processInvite(SharedMemory* shm, const Message &msg) {
    std::string target = msg.recipient;
    if (playersDB.find(target) == playersDB.end()) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Игрок %s не зарегистрирован.", target.c_str());
        strncpy(reply.data, buffer, sizeof(reply.data)-1);
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Приглашение: Игрок " << target << " не зарегистрирован." << std::endl;
        return;
    }
    if (onlinePlayers.find(target) == onlinePlayers.end()) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "Игрок %s не в сети.", target.c_str());
        strncpy(reply.data, buffer, sizeof(reply.data)-1);
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Приглашение: Игрок " << target << " не в сети." << std::endl;
        return;
    }
    Message inviteMsg = msg;
    sendSCMessage(shm, inviteMsg);
    std::cout << "[LOG] Приглашение от " << msg.sender << " отправлено игроку " << target << std::endl;
}

void loadPlayersDatabase() {
    std::ifstream in(PLAYERS_DB_FILE);
    if (!in.is_open()) {
        std::ofstream out(PLAYERS_DB_FILE);
        if (out.is_open()) {
            out << "admin:00000000\n";
            out.close();
        }
        playersDB["admin"] = "00000000";
        std::cout << "[LOG] Файл базы данных не найден. Создан файл с записью admin:00000000" << std::endl;
        return;
    }
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string login, password;
        if (std::getline(iss, login, ':') && std::getline(iss, password))
            playersDB[login] = password;
    }
    in.close();
    std::cout << "[LOG] Загружена база данных игроков." << std::endl;
}

void addPlayerToDatabase(const std::string& login, const std::string& password) {
    playersDB[login] = password;
    std::ofstream out(PLAYERS_DB_FILE, std::ios::app);
    if (out.is_open()) {
        out << login << ":" << password << "\n";
        out.close();
    }
}

void processRegister(SharedMemory* shm, const Message &msg) {
    std::string login = msg.sender;
    std::string password = msg.data;
    if (playersDB.find(login) != playersDB.end()) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Пользователь уже зарегистрирован. Пожалуйста, войдите.");
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Попытка регистрации существующего пользователя: " << login << std::endl;
        return;
    }
    addPlayerToDatabase(login, password);
    onlinePlayers.insert(login);
    std::cout << "[LOG] Новый игрок " << login << " зарегистрирован и вошёл в систему." << std::endl;
    Message reply;
    reply.type = MSG_GAME_UPDATE;
    strncpy(reply.sender, "server", sizeof(reply.sender)-1);
    strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
    strcpy(reply.data, "Регистрация прошла успешно. Вы вошли в систему.");
    sendSCMessage(shm, reply);
}

void processLogin(SharedMemory* shm, const Message &msg) {
    std::string login = msg.sender;
    std::string password = msg.data;
    if (playersDB.find(login) == playersDB.end()) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Пользователь не зарегистрирован. Пожалуйста, зарегистрируйтесь.");
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Попытка входа незарегистрированного пользователя: " << login << std::endl;
        return;
    }
    if (playersDB[login] != password) {
        Message reply;
        reply.type = MSG_GAME_UPDATE;
        strncpy(reply.sender, "server", sizeof(reply.sender)-1);
        strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
        strcpy(reply.data, "Неверный пароль!");
        sendSCMessage(shm, reply);
        std::cout << "[LOG] Неверный пароль для пользователя: " << login << std::endl;
        return;
    }
    onlinePlayers.insert(login);
    std::cout << "[LOG] Пользователь " << login << " успешно вошёл в систему." << std::endl;
    Message reply;
    reply.type = MSG_GAME_UPDATE;
    strncpy(reply.sender, "server", sizeof(reply.sender)-1);
    strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
    strcpy(reply.data, "Вы успешно вошли в систему.");
    sendSCMessage(shm, reply);
}

void processCreateGame(SharedMemory* shm, const Message &msg) {
    if (!currentGame.active) {
        currentGame.active = true;
        strncpy(currentGame.player1, msg.sender, sizeof(currentGame.player1)-1);
        initializeEmptyBoard(currentGame.board1);
        initializeEmptyBoard(currentGame.board2);
        for (int i = 1; i < 5; i++) {
            currentGame.player1Ships[i] = 0;
            currentGame.player2Ships[i] = 0;
        }
        currentGame.player1Ready = false;
        currentGame.player2Ready = false;
        std::cout << "[LOG] Игра создана игроком " << msg.sender << std::endl;
        sendGameUpdate(shm, msg.sender, "Игра создана. Ожидайте присоединения второго игрока.");
    } else {
        sendGameUpdate(shm, msg.sender, "Игра уже создана.");
    }
}

void processJoinGame(SharedMemory* shm, const Message &msg) {
    if (currentGame.active && strlen(currentGame.player2) == 0) {
        strncpy(currentGame.player2, msg.sender, sizeof(currentGame.player2)-1);
        std::cout << "[LOG] Игрок " << msg.sender << " присоединился к игре." << std::endl;
        sendGameUpdate(shm, currentGame.player1, "Второй игрок присоединился. Начните расстановку кораблей.");
        sendGameUpdate(shm, currentGame.player2, "Вы присоединились к игре. Начните расстановку кораблей.");
    } else {
        sendGameUpdate(shm, msg.sender, "Невозможно присоединиться к игре.");
    }
}

int main() {
    srand(time(NULL));
    loadPlayersDatabase();

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    if (ftruncate(shm_fd, sizeof(SharedMemory)) == -1) {
        perror("ftruncate");
        return 1;
    }
    SharedMemory* shm_ptr = (SharedMemory*) mmap(nullptr, sizeof(SharedMemory),
                                                 PROT_READ | PROT_WRITE, MAP_SHARED,
                                                 shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    pthread_mutexattr_t mutexAttr;
    pthread_mutexattr_init(&mutexAttr);
    pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shm_ptr->cs_queue.mutex, &mutexAttr);
    pthread_mutexattr_destroy(&mutexAttr);

    pthread_condattr_t condAttr;
    pthread_condattr_init(&condAttr);
    pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&shm_ptr->cs_queue.cond, &condAttr);
    pthread_condattr_destroy(&condAttr);

    shm_ptr->cs_queue.message_count = 0;
    shm_ptr->cs_queue.head = 0;
    shm_ptr->cs_queue.tail = 0;

    pthread_mutexattr_t mutexAttr2;
    pthread_mutexattr_init(&mutexAttr2);
    pthread_mutexattr_setpshared(&mutexAttr2, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shm_ptr->sc_queue.mutex, &mutexAttr2);
    pthread_mutexattr_destroy(&mutexAttr2);
    shm_ptr->sc_queue.tail = 0;

    std::cout << "[LOG] Сервер запущен. Ожидание сообщений от клиентов..." << std::endl;

    bool running = true;
    while (running) {
        pthread_mutex_lock(&shm_ptr->cs_queue.mutex);
        while (shm_ptr->cs_queue.message_count == 0)
            pthread_cond_wait(&shm_ptr->cs_queue.cond, &shm_ptr->cs_queue.mutex);
        Message msg = shm_ptr->cs_queue.messages[shm_ptr->cs_queue.head];
        shm_ptr->cs_queue.head = (shm_ptr->cs_queue.head + 1) % MESSAGE_QUEUE_SIZE;
        shm_ptr->cs_queue.message_count--;
        pthread_mutex_unlock(&shm_ptr->cs_queue.mutex);

        std::cout << "[LOG] Получено сообщение типа " << msg.type << " от " << msg.sender << std::endl;
        switch (msg.type) {
            case MSG_REGISTER:
                processRegister(shm_ptr, msg);
                break;
            case MSG_LOGIN:
                processLogin(shm_ptr, msg);
                break;
            case MSG_CREATE_GAME:
                processCreateGame(shm_ptr, msg);
                break;
            case MSG_JOIN_GAME:
                processJoinGame(shm_ptr, msg);
                break;
            case MSG_PLACE_SHIPS:
                processPlaceShips(shm_ptr, msg);
                break;
            case MSG_GAME_ACTION:
                processGameAction(shm_ptr, msg);
                break;
            case MSG_REQUEST_BOARD:
                sendBoardState(shm_ptr, msg.sender);
                std::cout << "[LOG] Запрос доски от " << msg.sender << std::endl;
                break;
            case MSG_INVITE:
                processInvite(shm_ptr, msg);
                break;
            case MSG_SHUTDOWN:
                if (std::string(msg.sender) == "admin") {
                    Message reply;
                    reply.type = MSG_GAME_UPDATE;
                    strncpy(reply.sender, "server", sizeof(reply.sender)-1);
                    strncpy(reply.recipient, "all", sizeof(reply.recipient)-1);
                    strcpy(reply.data, "Сервер отключается администратором.");
                    sendSCMessage(shm_ptr, reply);
                    std::cout << "[LOG] Администратор отключил сервер." << std::endl;
                    running = false;
                } else {
                    Message reply;
                    reply.type = MSG_GAME_UPDATE;
                    strncpy(reply.sender, "server", sizeof(reply.sender)-1);
                    strncpy(reply.recipient, msg.sender, sizeof(reply.recipient)-1);
                    strcpy(reply.data, "Отказано в отключении. Только admin может отключить сервер.");
                    sendSCMessage(shm_ptr, reply);
                    std::cout << "[LOG] Пользователь " << msg.sender << " пытался отключить сервер." << std::endl;
                }
                break;
            default:
                std::cout << "[LOG] Неизвестный тип сообщения." << std::endl;
                break;
        }
    }
    munmap(shm_ptr, sizeof(SharedMemory));
    shm_unlink(SHM_NAME);
    std::cout << "[LOG] Сервер завершил работу." << std::endl;
    return 0;
}
