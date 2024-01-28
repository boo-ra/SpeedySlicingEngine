cmake_minimum_required(VERSION 3.10)

#set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(meshnomoHPSE VERSION 0.1 DESCRIPTION "meshnomo High-Performance Slicing-Engine")

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(meshnomoHPSE
  hpse.cpp
  hpse.h
  parse_stl.cpp
  parse_stl.h
  slice.cpp
  slice.h
  write_files.cpp
  write_files.h
)


if (UNIX AND NOT APPLE)
set(LINUX TRUE)
endif()

if (LINUX)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9.0)
        set(USE_CPP17 TRUE)
    endif()
    target_compile_definitions(meshnomoHPSE PRIVATE _GLIBCXX_USE_CXX11_ABI=1)
endif()

if (USE_SHARED_LIBS)
    set(LIBRARY_LINK_TYPE shared)
    if (APPLE)
        set(LIBRARY_EXTENSION "dylib")
    elseif(LINUX)
        set(LIBRARY_EXTENSION "so")
    endif()
else()
    set(LIBRARY_LINK_TYPE static)
    set(LIBRARY_EXTENSION "a")
endif()


if (LINUX)
set_target_properties(meshnomoHPSE
                      PROPERTIES LINK_FLAGS
                      "-L${LIBRARY_PATH} -Wl,--no-as-needed,--enable-new-dtags,-rpath,.,-rpath,$ORIGIN,-rpath,$ORIGIN/.")
elseif(APPLE)
set_target_properties(meshnomoHPSE
                      PROPERTIES LINK_FLAGS
                      "-L${LIBRARY_PATH} -Wl,-rpath,.,-rpath,@rpath/.,-rpath,@rpath")
endif()

string(TOUPPER ${LIBRARY_LINK_TYPE} LIBRARY_LINK_TYPE)

if (USE_CPP17)
    target_compile_options(meshnomoHPSE PRIVATE -fPIC -std=c++17 -fopenmp -O3 -I/usr/local/include/)
    target_link_libraries(meshnomoHPSE PRIVATE PUBLIC -lstdc++fs -lpthread -lomp -L/usr/lib/llvm-11/lib/)
    #target_link_libraries(meshnomoHPSE PRIVATE PUBLIC -lcurl -lssl -lcrypto -lstdc++fs -lpthread -lomp -L/usr/lib/llvm-11/lib/)
else()
    target_compile_options(meshnomoHPSE PRIVATE -fPIC -std=c++14 -fopenmp -O3 -I/usr/local/include/)
    target_link_libraries(meshnomoHPSE PRIVATE PUBLIC -lpthread -lomp -L/usr/lib/llvm-11/lib/)
    #target_link_libraries(meshnomoHPSE PRIVATE PUBLIC -lcurl -lssl -lcrypto -lpthread -lomp -L/usr/lib/llvm-11/lib/)
endif()

set_target_properties(meshnomoHPSE PROPERTIES
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
