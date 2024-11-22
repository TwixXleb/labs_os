.PHONY: build test clean-build

build: clean-build
	git submodule init
	git submodule update
	cd ./build && cmake .. && make all

test:
	cd build && ./test_runner

clean-build:
	rm -rf ./build/
