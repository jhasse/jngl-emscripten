cmake_minimum_required(VERSION 3.6)

project(mygame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	set(USE_FLAGS "-s USE_SDL=2 -s USE_FREETYPE=1 -s FORCE_FILESYSTEM=0 -s USE_VORBIS=1 -s DISABLE_EXCEPTION_CATCHING=2 -s USE_LIBPNG=1 -s ASSERTIONS=1 --source-map-base 'http://127.0.0.1:8000/' ")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS} -s DISABLE_EXCEPTION_CATCHING=2 -s ASSERTIONS=1 --preload-file data -s INITIAL_MEMORY=1500MB --source-map-base 'http://127.0.0.1:8000/' ") #-s ALLOW_MEMORY_GROWTH=1  --preload-file data --preload-file intro
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

add_subdirectory(subprojects/jngl)


set(MYGAME_SANITIZE_ADDRESS_DEFAULT ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" OR ANDROID OR CMAKE_BUILD_TYPE MATCHES "Release")
	set(MYGAME_SANITIZE_ADDRESS_DEFAULT OFF)
endif()
option(MYGAME_SANITIZE_ADDRESS "Enable AddressSanitizer" ${MYGAME_SANITIZE_ADDRESS_DEFAULT})
if(MYGAME_SANITIZE_ADDRESS)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
endif()

file(GLOB SOURCES CONFIGURE_DEPENDS
	src/*.cpp
)
if(ANDROID)
	# build native_app_glue as a static lib
	set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
	add_library(native_app_glue STATIC
		${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

	# now build app's shared lib
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

	# Export ANativeActivity_onCreate(),
	# Refer to: https://github.com/android-ndk/ndk/issues/381.
	set(CMAKE_SHARED_LINKER_FLAGS
		"${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

	add_library(mygame SHARED ${SOURCES})

	target_include_directories(mygame PRIVATE
		${ANDROID_NDK}/sources/android/native_app_glue
		../../../../../../src
		../../../../../include
	)

	# add lib dependencies
	target_link_libraries(mygame
		android
		native_app_glue
		EGL
		GLESv3
		OpenSLES
		log
		z
		jngl
	)


	add_library(bootstrap SHARED android/app/src/main/cpp/bootstrap.cpp)

	target_include_directories(bootstrap PRIVATE
		${ANDROID_NDK}/sources/android/native_app_glue)

	# add lib dependencies
	target_link_libraries(bootstrap
		android
		native_app_glue
		log)
else()
	if(WIN32)
		add_executable(mygame ${SOURCES})
	else()
		add_executable(mygame ${SOURCES})
	endif()
	if(APPLE)
		target_link_libraries(mygame PRIVATE "-framework AppKit")
	elseif(NOT WIN32)
		# target_link_libraries(mygame PRIVATE
		# 	stdc++fs
		# )
	endif()
	target_link_libraries(mygame PRIVATE
		jngl
	)
endif()

file(GLOB GFX_FILES RELATIVE ${CMAKE_SOURCE_DIR}/data CONFIGURE_DEPENDS data/*.webp data/*.png
                                                                        data/*/*.webp)
file(GLOB SFX_FILES RELATIVE ${CMAKE_SOURCE_DIR}/data data/sfx/*.ogg)
# configure_file(src/engine/datafiles.hpp.in include/datafiles.hpp @ONLY)
target_include_directories(mygame PRIVATE ${CMAKE_BINARY_DIR}/include subprojects/cereal/include
                                           subprojects/spine-runtimes/spine-c/spine-c/include)

if(WIN32)
	target_compile_definitions(mygame PRIVATE _USE_MATH_DEFINES)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set_target_properties(mygame PROPERTIES WIN32_EXECUTABLE 1)
endif()
