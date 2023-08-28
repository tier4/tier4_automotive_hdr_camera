find_package(yaml-cpp REQUIRED)

#add_compile_options(-D X86_DEBUG)
add_compile_options(-fPIC)
add_compile_options(-D DEBUG)
set(CMAKE_CXX_FLAGS, "${CMAKE_CXX_FLAGS} -fPIC")

add_library(t4cam_tools SHARED  src/i2c.cpp src/t4cam_tools.cpp src/c2.cpp)
target_include_directories(t4cam_tools  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE ./
  src/)
include_directories(t4cam_tools ${YAML_INCLUDE_DIRS})
target_link_libraries(t4cam_tools ${YAML_CPP_LIBRARIES})
target_compile_options(t4cam_tools PRIVATE -fPIC)


install(TARGETS t4cam_tools EXPORT t4camConfig
  LIBRARY DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/t4cam)
install(EXPORT t4camConfig DESTINATION share/t4cam/cmake)
export(TARGETS t4cam_tools FILE t4camConfig.cmake)

option(USE_TEST "use test" OFF) # OFF by default

IF(USE_TEST)
find_package(GTest)
enable_testing()
message("gtest is found")
add_executable(test_t4cam_tools test/test.cpp t4cam_tools.cpp c2.cpp i2c.cpp)
target_link_libraries(test_t4cam_tools GTest::GTest GTest::Main)
target_include_directories(test_t4cam_tools  PUBLIC ./ ./include/)

add_test(NAME test_t4cam_tools COMMAND test_t4cam_tools)

ENDIF(USE_TEST)
