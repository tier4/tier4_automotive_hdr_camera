set(CMAKE_SYSTEM_NAME Linux) 
# Specify the cross compiler 
set(TOOLCHAIN "$ENV{TOOLCHAIN}") 
set(CMAKE_CXX_COMPILER "${TOOLCHAIN}/bin/aarch64-linux-gnu-g++") 
set(CMAKE_C_COMPILER "${TOOLCHAIN}/bin/aarch64-linux-gnu-gcc") 
# Targetfs path 
set(SYSROOT "$ENV{ROOTFS_DIR}") 
# Library paths 
set(LD_PATH "${SYSROOT}/usr/lib/aarch64-linux-gnu") 
set(LD_PATH_EXTRA "${SYSROOT}/lib/aarch64-linux-gnu") 
# setup compiler for cross-compilation 
set(CMAKE_CXX_FLAGS           "-fPIC"               CACHE STRING "c++ flags") 
set(CMAKE_C_FLAGS             "-fPIC"               CACHE STRING "c flags") 
set(CMAKE_SHARED_LINKER_FLAGS ""                    CACHE STRING "shared linker flags") 
set(CMAKE_MODULE_LINKER_FLAGS ""                    CACHE STRING "module linker flags") 
set(CMAKE_EXE_LINKER_FLAGS    ""                    CACHE STRING "executable linker flags") 
set(CMAKE_FIND_ROOT_PATH ${SYSROOT}) 
# Set compiler flags 
set(CMAKE_SHARED_LINKER_FLAGS   "--sysroot=${CMAKE_FIND_ROOT_PATH} -L${LD_PATH} -L${LD_PATH_EXTRA} -Wl,-rpath,${LD_PATH} -Wl,-rpath,${LD_PATH_EXTRA} -Wl,-rpath,${LD_PATH} -Wl,-rpath,${LD_PATH_EXTRA} ${CMAKE_SHARED_LINKER_FLAGS}") 
set(CMAKE_MODULE_LINKER_FLAGS   "--sysroot=${CMAKE_FIND_ROOT_PATH} -L${LD_PATH} -L${LD_PATH_EXTRA} -Wl,-rpath,${LD_PATH} -Wl,-rpath,${LD_PATH_EXTRA} -Wl,-rpath,${LD_PATH} -Wl,-rpath,${LD_PATH_EXTRA} ${CMAKE_SHARED_LINKER_FLAGS}") 
set(CMAKE_EXE_LINKER_FLAGS      "--sysroot=${CMAKE_FIND_ROOT_PATH} -L${LD_PATH} -L${LD_PATH_EXTRA} -Wl,-rpath,${LD_PATH} -Wl,-rpath,${LD_PATH_EXTRA} -Wl,-rpath,${LD_PATH_EXTRA} ${CMAKE_EXE_LINKER_FLAGS}") 
set(CMAKE_C_FLAGS "-fPIC --sysroot=${CMAKE_FIND_ROOT_PATH} -g" CACHE INTERNAL "" FORCE) 
set(CMAKE_CXX_FLAGS "-fPIC --sysroot=${CMAKE_FIND_ROOT_PATH} -fpermissive -g" CACHE INTERNAL "" FORCE) 
# Search for programs only in the build host directories 
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER) 
# Search for libraries and headers only in the target directories 
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY) 
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY) 

# set system default include dir
include_directories(BEFORE SYSTEM ${SYSROOT}/../include)
