.PHONY: build run test clean-build

build: clean-build
	mkdir build
	git submodule init
	git submodule update
	cd ./build && cmake .. && make all

run:
	./build/OS_LABS_exe

test:
	cd build && ./test_runner

clean-build:
	rm -rf ./build/
