
add_executable(set_gain_sample set_gain_sample.cpp)
target_link_libraries(set_gain_sample t4cam_tools)
target_include_directories(set_gain_sample PUBLIC ${PROJECT_SOURCE_DIR}/lib)
