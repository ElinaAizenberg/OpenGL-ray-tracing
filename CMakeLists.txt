cmake_minimum_required(VERSION 3.10)
project(project_6)

set(CMAKE_CXX_STANDARD 14)

# Add source files from the project
set(PROJECT_SRC
        main.cpp
)

include_directories(include)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})
