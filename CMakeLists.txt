cmake_minimum_required(VERSION 3.22)

set(BIN Qoinstr)
project(${BIN} VERSION 0.0.1 LANGUAGES CXX)

# build cpp_joinstr from source
add_custom_target(
    RunBeforeBuild ALL
    COMMAND bash joinstr.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    VERBATIM
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cpp_joinstr STATIC IMPORTED)
set_target_properties(cpp_joinstr PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libcpp_joinstr.a
)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

# include qontrol from git repo
include(FetchContent)
if(NOT DEFINED ENV{OFFLINE} OR "$ENV{OFFLINE}" STREQUAL "false")
    FetchContent_Declare(qontrol
        GIT_REPOSITORY https://github.com/pythcoiner/qontrol.git
        GIT_TAG 4444fc3
    )
else()
    FetchContent_Declare(qontrol
        SOURCE_DIR ./_deps/qontrol-src/
    )
endif()
FetchContent_MakeAvailable(qontrol)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    src/main.cpp
    src/AppController.cpp
    src/AccountController.cpp
    src/AccountWidget.cpp
    src/MainWindow.cpp
    src/MouseInspector.cpp
    src/widgets/ComboLine.cpp
    src/widgets/InputLine.cpp
    src/screens/common.cpp
    src/screens/Coins.cpp
    src/screens/Pools.cpp
    src/screens/Send.cpp
    src/screens/Receive.cpp
    src/screens/Settings.cpp
    src/screens/MenuTab.cpp
    src/screens/modals/CreatePool.cpp
    src/screens/modals/CreateAccount.cpp
    src/screens/modals/Stop.cpp
    src/screens/modals/SelectCoins.cpp
    src/payloads/Config.cpp
)

qt_add_executable(${BIN}
    ${SOURCES}
)

# ensure joinstr is built BEFORE Qoinstr
add_dependencies(${BIN} RunBeforeBuild)

add_dependencies(${BIN} 
    qontrol
)

target_include_directories(${BIN}
  PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
)

target_link_libraries(${BIN} 
    PRIVATE 
    Qt6::Gui 
    Qt6::Core 
    Qt6::Widgets 
    qontrol
    cpp_joinstr
)

set_target_properties(${BIN} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

