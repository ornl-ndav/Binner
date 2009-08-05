#ifndef _ATYPES_ 
#define _ATYPES_ 

/* for use with application code to reduce amount of manual work */

typedef struct projstruct
{
	/* these are for perspective */

	float fov; /* field of view, in degrees */
	float aspect; /* aspect ratio winx/winy */
	float hither;
	float yon;

	/* these are for orthogonal */
	float xmin, xmax;
	float ymin, ymax;
} projStruct;

typedef struct viewstruct
{
	float center[3];
	float eye[3];
	float up[3];
} viewStruct;

typedef struct lightstruct
{
	float lightpos[3];
} lightStruct;


typedef struct boundingBox3d
{
	float low[3];
	float high[3];
} bbox; 

extern void initApp (void);
extern void initGLsettings(void);
extern void reshape(int width,int height);
extern void keys(unsigned char c, int x, int y);
extern void mouse_handler(int button, int bstate, int x, int y);
extern void trackMotion(int x, int y);
extern int  initGLUT(int argc, char **argv);

#endif
