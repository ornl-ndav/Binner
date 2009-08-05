set(TOP ${PROJECT_SOURCE_DIR})

include_directories(
  ${TOP}/include 
)

add_definitions("-O3")

set(SRCS 
    dllist.c jval.c jrb.c fields.c 
    dmem.c exhash.c hilbert.c zcurve.c 
    intersect.c kernel.c raycasting.c
    nr.c nrcomplex.c nrutil.c 
    vcbbtree.c vcbcolor.c vcbcluster.c vcbdebug.c 
    vcbhist.c vcbfitting.c vcbimage.c vcblinalg.c 
    vcbmath.c vcbmcube.c vcbmntree.c 
    vcbops.c vcbrange.c vcbspcurve.c vcbtrimesh.c 
    vcbutils.c vcbvolren.c vbutrcast.c 
    vcblic.cpp Lic.cpp slic.cpp slic_aux.cpp
)

add_library(vcbnogl STATIC ${SRCS})

set(GLSRCS 
    ${SRCS}
    ArcBall.cpp glew.c splatting.c vcbglutils.c
)
  
#find_package(OPENGL REQUIRED)
#find_package(GLUT REQUIRED)
include(${TOP}/CMakeModules/OpenGLConfig.cmake)
				
include_directories(
   ${OPENGL_INCLUDE_DIR}
   ${GLUT_INCLUDE_DIR}
)

add_library(vcbgl STATIC ${GLSRCS})
target_link_libraries(vcbgl ${OPENGL_glu_LIBRARY} ${OPENGL_gl_LIBRARY} ${GLUT_LIBRARY})

if(NOT BINNER_INSTALL_DIR)
  set(DEST ${TOP})
else(NOT BINNER_INSTALL_DIR)
  set(DEST ${BINNER_INSTALL_DIR})
endif(NOT BINNER_INSTALL_DIR)

install(TARGETS vcbnogl vcbgl 
		LIBRARY DESTINATION ${DEST}/lib
		ARCHIVE DESTINATION ${DEST}/lib)
