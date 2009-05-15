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

find_path(OPENGL_INCLUDE_DIR gl.h
   /usr/local/include/GL
   /usr/local/include
   /usr/include/GL
   /usr/include
   )

find_library(OPENGL_gl_LIBRARY NAMES GL
             PATHS /usr/local/lib64 
				   /usr/lib64 
				   /usr/local/lib 
				   /usr/lib 
				   /usr/X11R6/lib) 

find_library(OPENGL_glu_LIBRARY NAMES GLU
             PATHS ${OPENGL_gl_LIBRARY} 
				   /usr/local/lib64 
				   /usr/lib64 
				   /usr/local/lib 
				   /usr/lib
				   /usr/X11R6/lib) 

#find_package(GLUT REQUIRED)
find_path(GLUT_INCLUDE_DIR glut.h
   /usr/local/include/GL
   /usr/local/include
   /usr/include/GL
   /usr/include
   )

find_library(GLUT_LIBRARY NAMES GLUT
             PATHS ${OPENGL_gl_LIBRARY} 
				   /usr/local/lib64 
				   /usr/lib64 
				   /usr/local/lib 
				   /usr/lib
				   /usr/X11R6/lib) 
				
include_directories(
#   ${OPENGL_INCLUDE_DIR}/Headers
   ${OPENGL_INCLUDE_DIR}
   ${GLUT_INCLUDE_DIR}
)

add_library(vcbgl STATIC ${GLSRCS})
target_link_libraries(vcbgl ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

install(TARGETS vcbnogl vcbgl 
		LIBRARY DESTINATION ${TOP}/lib
		ARCHIVE DESTINATION ${TOP}/lib)
