add_custom_target(ctrl)
add_dependencies(ctrl t4cam-ctrl)


find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
find_package(yaml-cpp REQUIRED)

add_executable(gui gui.cpp gui_callback.cpp)
target_include_directories(gui  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../lib/include>
  $<INSTALL_INTERFACE:.>
  PRIVATE ./)
include_directories(${GTKMM_INCLUDE_DIRS} ${YAML_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
target_link_libraries(gui ${GTKMM_LIBRARIES} ${YAML_CPP_LIBRARIES} t4cam_tools)


add_executable(t4cam-ctrl t4cam-ctrl.cpp)
target_link_libraries(t4cam-ctrl t4cam_tools ${YAML_CPP_LIBRARIES})
target_include_directories(t4cam-ctrl  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../lib/include>
  $<INSTALL_INTERFACE:.>
  PRIVATE ./)

install(TARGETS t4cam-ctrl RUNTIME  DESTINATION bin)
