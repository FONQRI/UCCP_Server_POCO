cmake_minimum_required(VERSION 2.8.12)

project(UCCP_Server)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(./src)




#seting file pathes
file(GLOB ServerHeaders ./src/server/*.h)
file(GLOB ServerCpp ./src/server/*.cpp)

file(GLOB ControllerHeaders ./src/controller/*.h)
file(GLOB ControllerCpp ./src/controller/*.cpp)

file(GLOB DatabaseHeaders ./src/database/*.h)
file(GLOB DatabaseCpp ./src/database/*.cpp)

file(GLOB TemplateEngineHeaders ./src/template_engine/*.h)
file(GLOB TemplateEngineCpp ./src/template_engine/*.cpp)

file(GLOB FireWallHeaders ./src/fire_wall/*.h)
file(GLOB FireWallCpp ./src/fire_wall/*.cpp)


#library
add_library(Server
  ${ServerHeaders}
  ${ServerCpp}
    )
add_library(Controller
  ${ControllerHeaders}
  ${ControllerCpp}
    )

add_library(Database
  ${DatabaseHeaders}
  ${DatabaseCpp}

    )

add_library(TemplateEngine
  ${TemplateEngineHeaders}
  ${TemplateEngineCpp}
    )

add_library(FireWall
  ${FireWallHeaders}
  ${FireWallCpp}
    )



set (CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++14")
find_package(Poco REQUIRED Foundation Util Net Data MongoDB Crypto)
add_executable(${PROJECT_NAME} "main.cpp" )


#linking
target_link_libraries(${PROJECT_NAME}
  Poco::Foundation
  Poco::Util
  Poco::Net
  Poco::Data
  Poco::MongoDB
  Poco::Crypto

    Server
    Controller
    Database
    TemplateEngine
    FireWall
)

target_link_libraries(Server
  Poco::Foundation
  Poco::Util
  Poco::Net
  Poco::Data
)


target_link_libraries(Database
  Poco::Foundation
  Poco::Util
  Poco::Net
  Poco::Data
  Poco::MongoDB

)



target_link_libraries(TemplateEngine
  Database
)



target_link_libraries(Controller
  Database
  Poco::Crypto

)


