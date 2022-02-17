# cmake build type, set to Debug or Release
TYPE?=Debug
LOAD_OBJ?="./data/stanford_bunny.obj"
EXEC=./build/sphere_mesh -o $(LOAD_OBJ) -s 15

CMAKE_FLAGS=-DCMAKE_BUILD_TYPE=$(TYPE)
all:
	mkdir -p build
	cd build && cmake .. $(CMAKE_FLAGS) && make -j

run: all
	$(EXEC)

debug: all
	gdb --args $(EXEC)

test: all
	./build/test_prio

clean:
	rm -r build/

.PHONY: all run clean
