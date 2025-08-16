cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT "Use Qt functionality" ON)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${CMAKE_PROJECT_NAME} MODULE)

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

if(ENABLE_FRONTEND_API)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
endif()

if(ENABLE_QT)
  find_package(Qt6 COMPONENTS Widgets Core)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
  target_compile_options(
    ${CMAKE_PROJECT_NAME}
    PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header -Wno-comma>
  )
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
  )
endif()

# ===== plugin-config.c を生成（.in を直接ビルドに入れない）=====
set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GEN_DIR}")

set(PLUGIN_CONFIG_IN "${CMAKE_CURRENT_SOURCE_DIR}/src/infra_shared/config/build/plugin-config.c.in")
set(PLUGIN_CONFIG_OUT "${GEN_DIR}/plugin-config.c")

configure_file("${PLUGIN_CONFIG_IN}" "${PLUGIN_CONFIG_OUT}" @ONLY)
set_source_files_properties("${PLUGIN_CONFIG_OUT}" PROPERTIES GENERATED TRUE)
# ===============================================================

target_sources(
  ${CMAKE_PROJECT_NAME}
  PRIVATE
    ${PLUGIN_CONFIG_OUT}
    src/app/plugin-main.cpp
    src/features/startup_check/app/StartupCheckFacade.cpp
    src/features/startup_check/domain/StartupCheckService.cpp
    src/features/startup_check/infrastructure/StdFsDirectoryChecker.cpp
    src/infra_shared/log/ObsLogger.c
    src/public_api/log/FoxclipLogApi.c
)

target_include_directories(
  ${CMAKE_PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/generated
)
set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME
                             ${_name}
)

# ===== ローカライズファイルのインストール =====
# 期待する配置: <repo>/data/obs-plugins/${_name}/locale/en-US.ini, ja-JP.ini, ...
install(
  TARGETS ${CMAKE_PROJECT_NAME}
  RUNTIME
    DESTINATION
      "obs-plugins/${_name}" # Windows
  LIBRARY
    DESTINATION
      "obs-plugins/${_name}" # macOS/Linux
)
# ===============================================
