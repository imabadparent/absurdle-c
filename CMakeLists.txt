######################################################################
# @author      : styx
# @file        : CMakeLists
# @created     : 2022-02-17T08:19:51-0500
######################################################################

cmake_minimum_required(VERSION 3.1)
project(absurdle C)
set(CMAKE_C_STANDARD 90)

include_directories("include/")
add_executable(absurdle src/main.c src/game.c)
install(TARGETS absurdle DESTINATION bin)
install(FILES lists/answers lists/words DESTINATION share/absurdle-c)

