# cmake build type, set to Debug or Release
TYPE?=Debug
LOAD_OBJ?="./data/cube.obj"

CMAKE_FLAGS=-DCMAKE_BUILD_TYPE=$(TYPE)
all:
	mkdir -p build
	cd build && cmake .. $(CMAKE_FLAGS) && make -j

run: all
	./build/sphere_mesh -o $(LOAD_OBJ)

debug: all
	gdb --args ./build/sphere_mesh -o $(LOAD_OBJ)

test: all
	./build/test_prio

clean:
	rm -r build/

.PHONY: all run clean
