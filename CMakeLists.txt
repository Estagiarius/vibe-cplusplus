cmake_minimum_required(VERSION 3.16)

project(AcademicManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network Test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Main Application
add_executable(AcademicManager
    src/main.cpp
    src/MainWindow.cpp
    src/DatabaseManager.cpp
    src/AcademicManager.cpp
    src/AIClient.cpp
    src/ManagementWidgets.cpp
    src/GradebookView.cpp
)

target_include_directories(AcademicManager PRIVATE src)

target_link_libraries(AcademicManager PRIVATE Qt6::Widgets Qt6::Sql Qt6::Network)

# Testing
enable_testing()
add_executable(TestLogic tests/test_logic.cpp src/DatabaseManager.cpp src/AcademicManager.cpp)
target_include_directories(TestLogic PRIVATE include src)
target_link_libraries(TestLogic PRIVATE Qt6::Test Qt6::Sql Qt6::Network)
add_test(NAME TestLogic COMMAND TestLogic)

add_executable(TestValidation tests/test_validation.cpp src/DatabaseManager.cpp src/AcademicManager.cpp)
target_include_directories(TestValidation PRIVATE include src)
target_link_libraries(TestValidation PRIVATE Qt6::Test Qt6::Sql Qt6::Network)
add_test(NAME TestValidation COMMAND TestValidation)
