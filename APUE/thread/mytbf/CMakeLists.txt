cmake_minimum_required(VERSION 3.15)

project(tbf C)
add_library(mytbfLib SHARED mytbf.c)
add_executable(tbf main.c)
target_link_libraries(tbf PUBLIC mytbfLib)
find_package(Threads REQUIRED)
target_link_libraries(tbf PRIVATE Threads::Threads)