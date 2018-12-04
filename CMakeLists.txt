cmake_minimum_required(VERSION 2.6)

project(liblogicalaccess-libnfc)

# Perform Conan basic setup to import targets etc
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

include(GenerateExportHeader)
# COPY PASTED FROM LLA MAIN REPO
macro(LLA_GENERATE_EXPORTS)
    string(TOLOWER ${ARGV0} _LOWERCASE_LIB_NAME)
    set(_EXPORT_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${_LOWERCASE_LIB_NAME}_api.hpp")

    generate_export_header(${PROJECT_NAME}
            EXPORT_MACRO_NAME "${ARGV0}_API"
            EXPORT_FILE_NAME ${_EXPORT_FILE_PATH})
    target_compile_definitions(${PROJECT_NAME} PRIVATE "${_LOWERCASE_LIB_NAME}_EXPORTS=1")

    if (MSVC)
        target_compile_options(${PROJECT_NAME} PUBLIC "/wd4251")
    endif ()

    # HEADER_INSTALL_DIR must be defined before calling the macro.
    install(FILES ${_EXPORT_FILE_PATH} DESTINATION ${HEADER_RELATIVE_INSTALL_DIR})
endmacro()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(CXX11 REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

    SET(VERSION_PRODUCTNAME_VALUE "Liblogicalaccess")
    SET(VERSION_PROJECT_FILE_DESCRIPTION_VALUE "${VERSION_PRODUCTNAME_VALUE}\ (${TARGET_ARCH})")
    add_definitions(-DVERSION_PROJECT_FILE_DESCRIPTION_VALUE="${VERSION_PROJECT_FILE_DESCRIPTION_VALUE}" -DVERSION_FILE_AND_PRODUCT_VALUE_STRING="${LIBLOGICALACCESS_VERSION_STRING}" -DVERSION_FILE_AND_PRODUCT_VALUE=${LIBLOGICALACCESS_WINDOWS_VERSION})
    file(GLOB LLA_RESOURCE_RC ${CMAKE_CURRENT_SOURCE_DIR}/vc++/resource.rc)
endif ()

add_subdirectory(plugins/logicalaccess/plugins/readers)

if (NOT DEFINED SYSCONF_INSTALL_DIR)
    set(SYSCONF_INSTALL_DIR etc)
endif ()
