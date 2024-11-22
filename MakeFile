.PHONY: build run test clean-build

build: clean-build
	mkdir build
	git submodule init
	git submodule update
	cd ./build && cmake .. && make all

run:
	./build/lab1

test:
	./build/LW1_test

clean-build:
	rm -rf ./build/