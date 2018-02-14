cmake_minimum_required(VERSION 2.6)
project(libnfc-nfcreaders)

if (WIN32)
  set(VERSION_PRODUCTNAME_VALUE "libnfc-nfcreaders")
  add_definitions(-DVERSION_PRODUCTNAME_VALUE="${VERSION_PRODUCTNAME_VALUE}")
endif()

if (WIN32)
  set(INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/../../../packages/include/logicalaccess/plugins/readers/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../packages/include/logicalaccess/plugins/cards/
  )
else ()
set(INCLUDE_FOLDER ${LIBLOGICALACCESS_INCLUDE_DIR} ${LIBLOGICALACCESS_INCLUDE_DIR}/logicalaccess/plugins/readers/
  ${LIBLOGICALACCESS_INCLUDE_DIR}/logicalaccess/plugins/cards/
  ${LIBLOGICALACCESS_INCLUDE_DIR}/logicalaccess/
    )
endif()
	
INCLUDE_DIRECTORIES(${INCLUDE_FOLDER})


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


set(INCLUDE_LIB ${include} ${include_core} ${include_readercardadapters}
${include_readers} ${include_commands} ${include_services})
set(SOURCE_LIB ${SRCS} ${source_readercardadapters} ${source_readers}
${source_commands} ${source_services})

add_library(
	libnfc-nfcreaders
	SHARED
	${SOURCE_LIB}
	${INCLUDE_LIB}
        ${resourcerc}
    )

SET_TARGET_PROPERTIES(libnfc-nfcreaders PROPERTIES VERSION ${LIBLOGICALACCESS_VERSION_STRING})


## Windows wants "liblogicalaccess"
## while Linux wants "logicalaccess"
if (UNIX)
set (LLA_NAME logicalaccess)
else ()
set (LLA_NAME liblogicalaccess)
endif()

target_link_libraries(
	libnfc-nfcreaders
	desfirecards
	mifarecards
	mifarepluscards
	mifareultralightcards
	iso7816readers
	iso7816cards
	iso15693cards
	${LLA_NAME}
	${LIBNFC_LIBRARIES}
)

install ( FILES ${include} DESTINATION include/logicalaccess/plugins/readers/nfc )
install ( FILES ${include_readercardadapters} DESTINATION include/logicalaccess/plugins/readers/nfc/readercardadapters )
install ( FILES ${include_commands} DESTINATION include/logicalaccess/plugins/readers/nfc/commands )
install ( TARGETS libnfc-nfcreaders RUNTIME DESTINATION bin/${LIB_SUFFIX} ARCHIVE DESTINATION lib/${LIB_SUFFIX} LIBRARY DESTINATION lib/${LIB_SUFFIX} )
if (MSVC)
	install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION pdb/${LIB_SUFFIX} OPTIONAL)
endif()
