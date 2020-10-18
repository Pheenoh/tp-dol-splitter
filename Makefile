run:
	g++ main.cpp -o main
	./main.exe

clean:
	@rm -rf GZ2J01/*
	@rm -rf GZ2E01/*
	@rm -rf GZ2P01/*
	@rm ./main.exe
	@rm ./main.exe.stackdump