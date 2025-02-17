#include <gtest/gtest.h>
#include <sstream>
#include <controller.h>
#include <tools.h>
#include <iostream>
#include <memory>
#include <thread>

extern std::shared_ptr<Node> root;

TEST(ControllerTest, CreateNodeTest) {
    std::stringstream stream;
    bool test = true;  // Установим в режим тестирования, чтобы не выводить лишние сообщения
    Controller(stream, test);

    // Вводим команду для создания узла
    std::string command = "create 1\n";
    std::string expected_output = "Ok\n";

    // Пропускаем вывод команды в строке
    stream.str(command);  // Инжектируем команду в поток

    // Проверяем, что командой create создается узел с ID 1
    std::shared_ptr<Node> node = FindNode(root, 1);
    ASSERT_NE(node, nullptr);  // Убедимся, что узел с ID 1 был создан
    ASSERT_EQ(node->id, 1);  // Проверим, что ID узла действительно равен 1

    // Проверяем корректный вывод
    ASSERT_EQ(stream.str(), expected_output);
}

TEST(ControllerTest, TimerStartStopTest) {
    std::stringstream stream;
    bool test = true;  // Включаем режим теста
    Controller(stream, test);

    // Вставляем команду для создания узла
    std::string create_command = "create 1\n";
    stream.str(create_command);
    Controller(stream, test);

    // Запуск таймера
    std::string start_command = "exec 1 start\n";
    stream.str(start_command);
    Controller(stream, test);

    std::shared_ptr<Node> node = FindNode(root, 1);
    ASSERT_NE(node, nullptr);  // Проверяем, что узел существует
    ASSERT_TRUE(node->start_time.has_value());  // Таймер должен быть запущен

    // Остановка таймера
    std::string stop_command = "exec 1 stop\n";
    stream.str(stop_command);
    Controller(stream, test);

    ASSERT_FALSE(node->start_time.has_value());  // Таймер должен быть остановлен
}

TEST(ControllerTest, TimerGetTimeTest) {
    std::stringstream stream;
    bool test = true;
    Controller(stream, test);

    // Создание узла и запуск таймера
    std::string create_command = "create 1\n";
    stream.str(create_command);
    Controller(stream, test);

    std::string start_command = "exec 1 start\n";
    stream.str(start_command);
    Controller(stream, test);

    std::shared_ptr<Node> node = FindNode(root, 1);
    ASSERT_NE(node, nullptr);  // Проверяем, что узел существует

    // Симуляция времени
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Останавливаем таймер
    std::string stop_command = "exec 1 stop\n";
    stream.str(stop_command);
    Controller(stream, test);

    // Получаем время
    std::string time_command = "exec 1 time\n";
    stream.str(time_command);
    Controller(stream, test);

    auto elapsed = GetElapsedTime(node);
    std::cout << "Elapsed time: " << elapsed.count() << "ms\n";  // Можно также вывести в консоль

    // Проверяем, что время больше 0 (прошло какое-то время)
    ASSERT_GT(elapsed.count(), 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
