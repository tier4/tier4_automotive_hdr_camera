add_custom_target(ctrl)
add_dependencies(ctrl t4cam-ctrl)


add_executable(t4cam-ctrl t4cam-ctrl.cpp)
target_link_libraries(t4cam-ctrl t4cam_tools)
target_include_directories(t4cam-ctrl  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../lib/include>
  $<INSTALL_INTERFACE:.>
  PRIVATE ./)

install(TARGETS t4cam-ctrl RUNTIME  DESTINATION bin)
