

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
				
