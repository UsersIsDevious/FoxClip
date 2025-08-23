cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT "Use Qt functionality" ON)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${CMAKE_PROJECT_NAME} MODULE)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE FOXCLIP_BUILDING)

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

# ==== nlohmann/json ====
option(FOXCLIP_FETCH_NLOHMANN_JSON "Fetch nlohmann/json via FetchContent" ON)

if(FOXCLIP_FETCH_NLOHMANN_JSON)
  include(FetchContent)
  FetchContent_Declare(nlohmann_json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
  FetchContent_MakeAvailable(nlohmann_json)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
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
    src/features/startup_check/infrastructure/StdFsDirectoryCreator.cpp
    src/features/plugin_loader/app/LoaderFacade.cpp
    src/features/plugin_loader/app/ValidationInputFactory.cpp
    src/features/plugin_loader/domain/ManifestValidator.cpp
    src/features/plugin_loader/infrastructure/NlohmannManifestReader.cpp
    src/features/plugin_loader/app/LoaderFacadeMulti.cpp
    src/infra_shared/fs/FsUtils.cpp
    src/infra_shared/fs/FileStore.cpp
    src/infra_shared/fs/PathResolver.cpp
    src/infra_shared/fs/DirectoryLister.cpp
    src/infra_shared/fs/roots/ObsConfigRootProvider.cpp
    src/infra_shared/ui/menu/app/ObsMenuRegistry.cpp
    src/infra_shared/log/ObsLogger.c
    src/infra_shared/config/path/ObsConfigPathProvider.cpp
    src/infra_shared/plugin/ModuleLoader.cpp
    src/infra_shared/plugin/HostApiV1.cpp
    src/infra_shared/plugin/ThreadedPlugin.cpp
    src/infra_shared/plugin/MultiPluginHost.cpp
    src/infra_shared/plugin/FoxclipPluginHost.cpp
    src/infra_shared/plugin/PluginFolderLogger.cpp
    src/infra_shared/startup/EnsurePluginDir.cpp
    src/infra_shared/version/Semver.cpp
    src/public_api/log/FoxclipLogApi.c
)

target_include_directories(
  ${CMAKE_PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>
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

# ==== Headers-only SDK（ビルドと同時に出力）====
set(FOXCLIP_SDK_NAME "foxclip-plugin-sdk")
set(FOXCLIP_SDK_DIR "${CMAKE_BINARY_DIR}/${FOXCLIP_SDK_NAME}")
set(FOXCLIP_SDK_INCLUDE_DIR "${FOXCLIP_SDK_DIR}/include")
set(FOXCLIP_SDK_CMAKE_DIR "${FOXCLIP_SDK_DIR}/cmake/FoxclipPluginHeaders")

# SDK用テンプレを先に生成（configure段階で行う）
set(_sdk_tmp "${CMAKE_BINARY_DIR}/_sdk_tmp")
file(MAKE_DIRECTORY "${_sdk_tmp}")

# これら2つの .in ファイルをリポジトリに追加してください（下に中身あり）
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FoxclipPluginHeaders/FoxclipPluginHeadersConfig.cmake.in"
  "${_sdk_tmp}/FoxclipPluginHeadersConfig.cmake"
  @ONLY
)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FoxclipPluginHeaders/FoxclipPluginHeadersTargets.cmake.in"
  "${_sdk_tmp}/FoxclipPluginHeadersTargets.cmake"
  @ONLY
)

add_custom_target(
  sdk
  ALL
  COMMAND "${CMAKE_COMMAND}" -E rm -rf "${FOXCLIP_SDK_DIR}"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${FOXCLIP_SDK_INCLUDE_DIR}"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${FOXCLIP_SDK_CMAKE_DIR}"
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/include" "${FOXCLIP_SDK_INCLUDE_DIR}"
  COMMAND
    "${CMAKE_COMMAND}" -E copy "${_sdk_tmp}/FoxclipPluginHeadersConfig.cmake"
    "${FOXCLIP_SDK_CMAKE_DIR}/FoxclipPluginHeadersConfig.cmake"
  COMMAND
    "${CMAKE_COMMAND}" -E copy "${_sdk_tmp}/FoxclipPluginHeadersTargets.cmake"
    "${FOXCLIP_SDK_CMAKE_DIR}/FoxclipPluginHeadersTargets.cmake"
  COMMENT "Staging headers-only SDK to ${FOXCLIP_SDK_DIR}"
)

# 任意：ZIP も作るなら
set(FOXCLIP_SDK_ZIP "${CMAKE_BINARY_DIR}/${FOXCLIP_SDK_NAME}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}.zip")
add_custom_target(
  sdk_zip
  COMMAND "${CMAKE_COMMAND}" -E tar "cf" "${FOXCLIP_SDK_ZIP}" --format=zip "${FOXCLIP_SDK_NAME}"
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  DEPENDS sdk
  COMMENT "Packaging SDK zip: ${FOXCLIP_SDK_ZIP}"
)
