run: build
	./main.exe
	make clean

build:
	clang++ -c transport_catalogue.cpp -std=c++17 -Wno-everything
	clang++ -c stat_reader.cpp -std=c++17 -Wno-everything
	clang++ -c input_reader.cpp -std=c++17 -Wno-everything
	clang++ -c main.cpp -std=c++17 -Wno-everything
	clang++ -o main.exe transport_catalogue.o input_reader.o stat_reader.o main.o -std=c++17 -Wno-everything

clean:
	del main.exe transport_catalogue.o input_reader.o main.o stat_reader.o