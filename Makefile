build:
	rm -rf ./build/
	mkdir build
	git submodule init
	git submodule update
	cd ./build; cmake ..; make all

test:
	cd ./build; ./test &
	sleep 30
	ps aux

check:
	ls -l ./build/output1.txt
	ls -l ./build/output2.txt

print:
	echo "Contents of output1.txt:"
	cat ./build/output1.txt
	echo " "
	echo "Contents of output2.txt:"
	cat ./build/output2.txt
