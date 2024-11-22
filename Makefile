.PHONY: build run test clean-build

build: clean-build
	mkdir build
	git submodule init
	git submodule update
	cd ./build && cmake .. && make all

run:
	./build/OS_LABS_exe

test:
	./test_runner > test_output.log 2>&1

clean-build:
	rm -rf ./build/
