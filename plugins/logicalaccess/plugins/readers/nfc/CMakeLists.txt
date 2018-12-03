cmake_minimum_required(VERSION 2.6)
project(libnfc-nfcreaders)

if (MSVC)
    set(VERSION_PRODUCTNAME_VALUE "libnfc-nfcreaders")
    add_definitions(-DVERSION_PRODUCTNAME_VALUE="${VERSION_PRODUCTNAME_VALUE}")
endif ()

set(HEADER_RELATIVE_INSTALL_DIR "include/logicalaccess/plugins/readers/private-pcsc")

file(GLOB SRCS *.cpp)
file(GLOB include *.hpp)

file(GLOB include_readercardadapters ${CMAKE_CURRENT_SOURCE_DIR}/readercardadapters/*.hpp)
SOURCE_GROUP("Header Files\\readercardadapters" FILES ${include_readercardadapters})

file(GLOB source_readercardadapters ${CMAKE_CURRENT_SOURCE_DIR}/readercardadapters/*.cpp)
SOURCE_GROUP("Source Files\\readercardadapters" FILES ${source_readercardadapters})

file(GLOB include_commands ${CMAKE_CURRENT_SOURCE_DIR}/commands/*.hpp)
SOURCE_GROUP("Header Files\\commands" FILES ${include_commands})

file(GLOB source_commands ${CMAKE_CURRENT_SOURCE_DIR}/commands/*.cpp)
SOURCE_GROUP("Source Files\\commands" FILES ${source_commands})


set(INCLUDE_LIB ${include} ${include_readercardadapters} ${include_commands} ${include_services})
set(SOURCE_LIB ${SRCS} ${source_readercardadapters} ${source_commands})

add_library(libnfc-nfcreaders
        SHARED
        ${SOURCE_LIB}
        ${INCLUDE_LIB}
        ${LLA_RESOURCE_RC})
LLA_GENERATE_EXPORTS(LLA_READERS_NFC_NFC)

SET_TARGET_PROPERTIES(libnfc-nfcreaders PROPERTIES VERSION ${LIBLOGICALACCESS_VERSION_STRING})

target_link_libraries(libnfc-nfcreaders PUBLIC
        CONAN_PKG::LogicalAccess
        CONAN_PKG::LibNFC)
target_include_directories(libnfc-nfcreaders PUBLIC ${CMAKE_SOURCE_DIR}/plugins)

install(FILES ${include} DESTINATION include/logicalaccess/plugins/readers/nfc)
install(FILES ${include_readercardadapters} DESTINATION include/logicalaccess/plugins/readers/nfc/readercardadapters)
install(FILES ${include_commands} DESTINATION include/logicalaccess/plugins/readers/nfc/commands)
install(TARGETS libnfc-nfcreaders RUNTIME DESTINATION bin/${LIB_SUFFIX} ARCHIVE DESTINATION lib/${LIB_SUFFIX} LIBRARY DESTINATION lib/${LIB_SUFFIX})
if (MSVC)
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION pdb/${LIB_SUFFIX} OPTIONAL)
endif ()
