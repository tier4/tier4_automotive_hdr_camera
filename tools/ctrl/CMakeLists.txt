add_custom_target(ctrl)
add_dependencies(ctrl t4cam-ctrl)


find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

find_package(Threads)
find_package(yaml-cpp REQUIRED)
add_compile_options(-fPIC)
add_executable(t4cam-ctrl-gui gui.cpp gui_c2.cpp gui_callback.cpp gui_callback_c2.cpp Slider.cpp)
target_include_directories(t4cam-ctrl-gui  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../lib/include>
  $<INSTALL_INTERFACE:.>
  PRIVATE ./)
include_directories(${GTKMM_INCLUDE_DIRS} ${YAML_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
target_link_libraries(t4cam-ctrl-gui ${GTKMM_LIBRARIES} ${YAML_CPP_LIBRARIES}.a ${CMAKE_THREAD_LIBS_INIT}  t4cam_tools)


add_executable(t4cam-ctrl t4cam-ctrl.cpp c1.cpp c2.cpp)
target_link_libraries(t4cam-ctrl ${YAML_CPP_LIBRARIES}.a t4cam_tools)
target_include_directories(t4cam-ctrl  PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../lib/include>
  $<INSTALL_INTERFACE:.>
  PRIVATE ./)

install(TARGETS t4cam-ctrl RUNTIME  DESTINATION bin)
install(TARGETS t4cam-ctrl-gui RUNTIME  DESTINATION bin)
