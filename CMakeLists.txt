#GUI VERSION
cmake_minimum_required(VERSION 2.6)
project (Reassembler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(SFML 2 COMPONENTS graphics system window REQUIRED)
find_package(SFGUI 0.3 REQUIRED)

include_directories(include)

file(GLOB Reassembler_SRCS 	
	${PROJECT_SOURCE_DIR}/src/*.cpp
	${PROJECT_SOURCE_DIR}/include/*.h
)

add_executable(Reassembler ${Reassembler_SRCS})

if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(Reassembler ${SFML_LIBRARIES})
endif()

if(SFGUI_FOUND)
	include_directories(${SFGUI_INCLUDE_DIR})
	target_link_libraries(Reassembler ${SFGUI_LIBRARY})
endif()


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ships DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.txt DESTINATION ${CMAKE_BINARY_DIR})
