# cmake build type, set to Debug or Release
TYPE?=Debug


CMAKE_FLAGS=-DCMAKE_BUILD_TYPE=$(TYPE)
all:
	mkdir -p build && cd build && cmake .. $(CMAKE_FLAGS) && make -j

run: all
	./build/sphere_mesh

debug: all
	gdb ./build/sphere_mesh

clean:
	rm -r build/

.PHONY: all run clean
