CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

PROJECT(HelloWorld)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_BUILD_TYPE "Debug")

set(EXTRA_LIBS ${CMAKE_CURRENT_LIST_DIR}/lib/libUnitreeMotorSDK_Linux64.so)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    /usr/include/PHOENIX
)

add_executable(HelloWorld
    src/main.cpp
    src/pid/pid.cpp
    src/motor/motor.cpp
)

add_executable(remote src/remote.cpp)

add_executable(graph src/graph.cpp)

target_link_libraries(HelloWorld
    ${OpenCV_LIBS}
    ${EXTRA_LIBS}
    ${Boost_LIBRARIES}
    -lpthread
)

target_link_libraries(remote  ${Boost_LIBRARIES} -lPHOENIX -lpthread)

target_link_libraries(graph ${OpenCV_LIBS} ${Boost_LIBRARIES} -lpthread -lPHOENIX)