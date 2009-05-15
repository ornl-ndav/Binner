set(TOP ${PROJECT_SOURCE_DIR})

include_directories(
  ${TOP}/include 
)

add_definitions("-O3")

link_libraries(vcbnogl m)

add_executable(vcbcomp vcbcomp.c)
add_executable(vcbrcast vcbrcast.c)
add_executable(vcbhead vcbhead.c)
add_executable(vcbrange vcbrange.c)
add_executable(vcbcut vcbcut.c)

if(NOT BINNER_INSTALL_DIR)
  set(DEST ${TOP})
else(NOT BINNER_INSTALL_DIR)
  set(DEST ${BINNER_INSTALL_DIR})
endif(NOT BINNER_INSTALL_DIR)

install(TARGETS vcbcomp vcbrcast vcbhead vcbrange vcbcut
		RUNTIME DESTINATION ${DEST}/bin
		LIBRARY DESTINATION ${DEST}/lib
		ARCHIVE DESTINATION ${DEST}/lib)
