.PHONY: build run test clean-build

build: clean-build
	mkdir build
	git submodule init
	git submodule update
	cd ./build && cmake .. && make all

run:
	./build/OS_LABS_exe

test:
	./build/OS_LABS_test

clean-build:
	rm -rf ./build/
