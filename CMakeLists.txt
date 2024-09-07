cmake_minimum_required(VERSION 3.10)

project(SewerBreakout)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(./)

file(GLOB sourcefiles "*.h" "*.cpp")

add_executable(SewerBreakout ${sourcefiles})

target_link_libraries(SewerBreakout PUBLIC allegro)
target_link_libraries(SewerBreakout PUBLIC allegro_dialog)
target_link_libraries(SewerBreakout PUBLIC allegro_image)
target_link_libraries(SewerBreakout PUBLIC allegro_primitives)
target_link_libraries(SewerBreakout PUBLIC allegro_font)
target_link_libraries(SewerBreakout PUBLIC allegro_audio)
target_link_libraries(SewerBreakout PUBLIC allegro_ttf)
target_link_libraries(SewerBreakout PUBLIC allegro_acodec)


