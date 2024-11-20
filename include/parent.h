#ifndef PARENT_H
#define PARENT_H

#include <string>
#include <sys/types.h>

class Parent {
        public:
        Parent(const std::string& inputFile);
        void run();

        private:
        std::string inputFileName;
        pid_t child1_pid;
        pid_t child2_pid;
        int pipe1_fd[2]; // Pipe for child 1
        int pipe2_fd[2]; // Pipe for child 2

        void createChildren();
        void processStrings();
        void sendStringToChild(const std::string& str, int childNum);
};

#endif // PARENT_H
