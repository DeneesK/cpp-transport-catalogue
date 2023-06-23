run: build
	./main.exe
	make clean

build:
	clang++ -c transport_catalogue.cpp -std=c++17
	clang++ -c json_reader.cpp -std=c++17
	clang++ -c domain.cpp -std=c++17
	clang++ -c geo.cpp -std=c++17
	clang++ -c svg.cpp -std=c++17
	clang++ -c json.cpp -std=c++17
	clang++ -c request_handler.cpp -std=c++17
	clang++ -c map_renderer.cpp -std=c++17
	clang++ -c main.cpp -std=c++17
	clang++ -o main.exe domain.o transport_catalogue.o geo.o json.o json_reader.o request_handler.o map_renderer.o svg.o main.o -std=c++17
	del transport_catalogue.o geo.o json.o json_reader.o request_handler.o map_renderer.o domain.o svg.o main.o

clean:
	del main transport_catalogue.o geo.o json.o json_reader.o request_handler.o map_renderer.o domain.o svg.o main.o domain.o