all:
	mkdir -p build/ && cd build/ && cmake ../ && make

run: all
	./build/sphere_mesh
