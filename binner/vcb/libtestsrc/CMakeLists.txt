set(TOP ${PROJECT_SOURCE_DIR})

include_directories(
  ${TOP}/include 
)

add_definitions("-O3")

link_libraries(vcbnogl)

add_executable(vcbcomp vcbcomp.c)
add_executable(vcbrcast vcbrcast.c)
add_executable(vcbhead vcbhead.c)
add_executable(vcbrange vcbrange.c)
add_executable(vcbcut vcbcut.c)

install(TARGETS vcbcomp vcbrcast vcbhead vcbrange vcbcut
		RUNTIME DESTINATION ${TOP}/bin
		LIBRARY DESTINATION ${TOP}/lib
		ARCHIVE DESTINATION ${TOP}/lib)
