cmake_minimum_required(VERSION 3.20)
project(VZIServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(HTTPLIB_REQUIRE_OPENSSL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.18.3
)
FetchContent_MakeAvailable(httplib)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

add_executable(vzi_server src/main.cpp)
target_link_libraries(vzi_server PRIVATE httplib::httplib nlohmann_json::nlohmann_json)
if (WIN32)
  target_link_libraries(vzi_server PRIVATE ws2_32)
endif()
