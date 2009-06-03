set(TOP ${PROJECT_SOURCE_DIR})

include_directories(
  ${TOP}/include 
  ${TOP}/vcb/include
)

#set(LIBS vcbnogl)
#add_library(${LIBS} STATIC IMPORTED)
#set_property(
#	TARGET 
#	${LIBS} PROPERTY IMPORTED_LOCATION ${TOP}/vcb/lib/libvcbnogl.a
#)

set(EXES 
	gmeshrebin bmeshrebin largebmeshrebin bounds bslicer 
	clip3d clipvoxel compvol collector counts
	export2vtk export2vcb 
	gencell getfrom giveto 
	netrebin pix2mesh 
	rebin rot3d 
	scale3d serve sorter tran3d
	seeall seepara seebmeshvol seebmeshvolc)

link_libraries(vcbnogl m)
add_executable(netrebin 
               netrebin.c socketfun.c binner.c voxelize.c 
			   volume.c clip.c cell.c geometry.c binnerio.c)
add_executable(getfrom getfrom.c socketfun.c)
add_executable(giveto giveto.c socketfun.c)
add_executable(serve serve.c socketfun.c)

add_executable(pix2mesh pix2mesh.c cell.c binnerio.c)
add_executable(gencell gencell.c cell.c binnerio.c)
add_executable(rot3d rot3d.c binnerio.c)
add_executable(tran3d tran3d.c binnerio.c)
add_executable(scale3d scale3d.c binnerio.c)
add_executable(clip3d clip3d.c clip.c geometry.c binnerio.c)

add_executable(clipvoxel 
               clipvoxel.c volume.c clip.c cell.c geometry.c binnerio.c)

add_executable(compvol 
               compvol.c volume.c clip.c cell.c geometry.c binnerio.c)
			   
add_executable(rebin 
			   rebin.c binner.c voxelize.c volume.c 
			   clip.c cell.c geometry.c  binnerio.c)

add_executable(bounds bounds.c cell.c binnerio.c)
add_executable(counts counts.c binnerio.c)

add_executable(bmeshrebin 
               bmeshrebin.c rebinapp.c binner.c voxelize.c volume.c 
               clip.c cell.c geometry.c binnerio.c)

add_executable(gmeshrebin 
               gmeshrebin.c rebinapp.c binner.c voxelize.c volume.c 
               clip.c cell.c geometry.c binnerio.c)

add_executable(largebmeshrebin 
               largebmeshrebin.c binner.c voxelize.c volume.c 
			   clip.c cell.c geometry.c binnerio.c)

add_executable(export2vtk export2vtk.c binnerio.c)
add_executable(export2vcb export2vcb.c binnerio.c)
add_executable(bslicer bslicer.c  binnerio.c)

find_library(PTHREAD_LIBRARY NAMES pthread)
add_executable(collector collector.c socketfun.c)
target_link_libraries(collector ${PTHREAD_LIBRARY})
add_executable(sorter sorter.c socketfun.c binnerio.c)
target_link_libraries(sorter ${PTHREAD_LIBRARY})

#find_package(OPENGL REQUIRED)
#find_package(GLUT REQUIRED)

include(${TOP}/vcb/CMakeModules/OpenGLConfig.cmake)

include_directories(
#   ${OPENGL_INCLUDE_DIR}/Headers
   ${OPENGL_INCLUDE_DIR}
   ${GLUT_INCLUDE_DIR}
)

link_libraries(vcbgl)

add_executable(seeall seeall.cpp formain.cpp binnerio.c)
add_executable(seepara seepara.cpp formain.cpp cell.c binnerio.c)
add_executable(seebmeshvol seebmeshvol.cpp formain.cpp cell.c binnerio.c)
add_executable(seebmeshvolc seebmeshvolc.cpp formain.cpp cell.c binnerio.c)

if(NOT BINNER_INSTALL_DIR)
  set(DEST ${TOP})
else(NOT BINNER_INSTALL_DIR)
  set(DEST ${BINNER_INSTALL_DIR})
endif(NOT BINNER_INSTALL_DIR)

install(TARGETS ${EXES}
		RUNTIME DESTINATION ${DEST}/bin
		LIBRARY DESTINATION ${DEST}/lib
		ARCHIVE DESTINATION ${DEST}/lib)
