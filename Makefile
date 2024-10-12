.PHONY: build run test

all: parent_exe child_exe

parent_exe: parent.c
    gcc -o parent_exe parent.c -I./include

child_exe: child.c
    gcc -o child_exe child.c -I./include

build: clean-build
	mkdir build
	git submodule init
	git submodule update
	cd ./build; cmake ..; make all

run:
	./build/*_exe

test:
	./build/*_test

clean-build:
	rm -rf ./build/