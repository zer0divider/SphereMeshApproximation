cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 11)
project(sphere_mesh)

add_subdirectory(src/zer0engine/)

cmake_policy(SET CMP0072 NEW)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

set(SOURCE_FOLDER src)
set(PROJECT_SOURCES
	main.cpp
	ModelViewer.cpp
	ModelViewer.h
	CmdParser.h
	CmdParser.cpp
	DiffuseShader.h
	DiffuseShader.cpp
	DynamicMesh.h
	DynamicMesh.cpp
	BackReferenceList.h
	SphereMesh.cpp
	SphereMesh.h
	ModeSwitcher.h
)

# add source folder prefix
set(PROJECT_SOURCES_FULL_PATH "")
foreach(i ${PROJECT_SOURCES})
	LIST(APPEND PROJECT_SOURCES_FULL_PATH "${SOURCE_FOLDER}/${i}")
endforeach(i)

# include directories
include_directories(
	src/
	${OPENGL_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
	${FREETYPE_INCLUDE_DIRS}
)

# specify link libraries
set(LIBRARIES
	${SDL2_LIBRARIES}
	${OPENGL_LIBRARIES}
	${FREETYPE_LIBRARIES}
)

# executable
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES_FULL_PATH})

# tests
add_executable(test_prio tests/test_prio.cpp src/DynamicMesh.cpp )
target_link_libraries(test_prio zer0engine ${LIBRARIES})

# set link libraries
target_link_libraries(${CMAKE_PROJECT_NAME} zer0engine ${LIBRARIES})
