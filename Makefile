.PHONY: build test

build: clean-build
	mkdir build
	git submodule init
	git submodule update
	cd ./build; cmake ..; make all

test:
	./build/*_test

clean-build:
	rm -rf ./build/