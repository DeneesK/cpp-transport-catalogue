run: build
	./main.exe
	make clean

build:
	clang++ -c transport_catalogue.cpp -std=c++17 -Wno-everything
	clang++ -c json_reader.cpp -std=c++17 -Wno-everything
	clang++ -c geo.cpp -std=c++17 -Wno-everything
	clang++ -c json.cpp -std=c++17 -Wno-everything
	clang++ -c request_handler.cpp -std=c++17 -Wno-everything
	clang++ -c main.cpp -std=c++17 -Wno-everything
	clang++ -o main.exe transport_catalogue.o geo.o json.o json_reader.o request_handler.o main.o -std=c++17 -Wno-everything
	del transport_catalogue.o geo.o main.o json.o json_reader.o request_handler.o

clean:
	del main.exe transport_catalogue.o geo.o main.o json.o json_reader.o request_handler.o