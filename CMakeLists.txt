cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)
project(sphere_mesh)

add_subdirectory(zer0engine/)

cmake_policy(SET CMP0072 NEW)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

set(PROJECT_SOURCES
	main.cpp
	ModelViewer.cpp
	ModelViewer.h
	CmdParser.h
	CmdParser.cpp
	DiffuseShader.h
	DiffuseShader.cpp
)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
	${FREETYPE_INCLUDE_DIRS}
)

set(LIBRARIES
	${SDL2_LIBRARIES}
	${OPENGL_LIBRARIES}
	${FREETYPE_LIBRARIES}
)

# executable
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES} ${MIDDLE_SOURCES}
				 ${RESOURCE_GENERATED_LIST_FILE})

# set link libraries
target_link_libraries(${CMAKE_PROJECT_NAME} zer0engine ${LIBRARIES})
