cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
project(zer0engine)

# set policy to use new opengl libraries
cmake_policy(SET CMP0072 NEW)

# looking for libraries (SDL2, SDL2_image, OpenGL, freetype)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

set(ENGINE_SOURCES
	zer0engine.cpp
	zer0engine.h
	zSingleton.h
	zFramework.cpp
	zFramework.h
	zLogger.cpp
	zLogger.h
	zQuaternion.h
	zQuaternion.cpp
	zVector4D.h
	zVector3D.cpp
	zVector3D.h
	zVector2D.cpp
	zVector2D.h
	zMatrix4.cpp
	zMatrix4.h
	zColor.cpp
	zColor.h
	zShader.h
	zShader.cpp
	zMesh.cpp
	zMesh.h
	zTexture.cpp
	zTexture.h
	zMath.h
	zMath.cpp
	zRect.cpp
	zRect.h
	zFont.cpp
	zFont.h
	zGlyphMap.cpp
	zGlyphMap.h
	zTextView.cpp
	zTextView.h
	zConfig.h

	glew/glew.c
)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
	${FREETYPE_INCLUDE_DIRS}
)

add_library(zer0engine ${ENGINE_SOURCES})
