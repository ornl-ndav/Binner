#include <stdio.h>
#include <stdlib.h>
#include "vcbutils.h"
#include "vcbimage.h"

void vcbImgWritePPM(char *filename, unsigned char * fbuffer, int nchannels, int width, int height)
{
	int i, npixels;
	FILE * fp;

	if (nchannels < 3) {
		fprintf(stderr,"PPM requires at least 3 color channels, not %d (input)\n",nchannels);
		return;
	}
  
	if((fp=fopen(filename,"w")) == NULL) {
		perror(filename);     
		return;
    }

	fprintf(fp,"P6 %d %d 255",width,height);
	npixels = width * height;

	/* store all pixel data in byte format */
	for (i = 0; i < npixels; i ++)
		fwrite(&fbuffer[i*nchannels], sizeof(unsigned char), 3, fp);

	fclose(fp);
}

void vcbImgWritePGM(char *filename, unsigned char * fbuffer, int nchannels, int width, int height)
{
	int i, npixels;
	FILE * fp;

	if (nchannels < 1) {
		fprintf(stderr,"PGM requires at least one color channels, not %d (input)\n",nchannels);
		return;
	}
  
	if((fp=fopen(filename,"w")) == NULL) {
		perror(filename);     
		return;
    }

	fprintf(fp,"P5\n%d %d\n255\n",width,height);
	npixels = width * height;

	/* store all pixel data in byte format */
	for (i = 0; i < npixels; i ++)
		fwrite(&fbuffer[i*nchannels], sizeof(unsigned char), 1, fp);

	fclose(fp);
}


/* ------------------------------------------------------------------------- */
/* By Chris Johnson, Jian Huang
 *
 * These functions make writing the BMP, TGA image files out much simpler.  
 * Since fwrite only takes pointers as input, functions like these allow us to
 * use literals and call these functions instead of declaring variables for
 * all our data.  The functions whose types are spelled in reverse switch
 * byte order of their arguments and then write them to the file. 
 *
 * INTERNAL USE ONLY.
 */
static void write_char (FILE * fp, char c) {
   fwrite (&c, sizeof (char), 1, fp);
}

static void write_short_reverse (FILE * fp, short c) {
   vcbToggleEndian(&c, sizeof(short));
   fwrite (&c, sizeof (short), 1, fp);
}

static void write_short_normal (FILE * fp, short c) {
   fwrite (&c, sizeof (short), 1, fp);
}

static void write_long_reverse (FILE * fp, long c) {
   vcbToggleEndian(&c, sizeof(long));
   fwrite (&c, sizeof (long), 1, fp);
}

static void write_long_normal (FILE * fp, long c) {
   fwrite (&c, sizeof (long), 1, fp);
}

static void write_int_reverse (FILE * fp, int c) {
   vcbToggleEndian(&c, sizeof(int));
   fwrite (&c, sizeof (int), 1, fp);
}

static void write_int_normal (FILE * fp, int c) {
   fwrite (&c, sizeof (int), 1, fp);
}

/* end of intenal functions */

void vcbImgWriteTGA(char *filename, unsigned char * fbuffer, int nchannels, int width, int height)
{
	int i, npixels;
	unsigned int  code;
	unsigned char bits;	
	FILE * fp;
    void (* write_short) (FILE *, short);
    void (* write_int) (FILE *, int);
    void (* write_long) (FILE *, long);

	if (nchannels < 3) {
		fprintf(stderr,"TGA requires at least 3 color channels, not %d (input)\n",nchannels);
		return;
	}
  
	if((fp=fopen(filename,"wb")) == NULL) {
		perror(filename);     
		return;
    }

	/* as far as I know, TGA seems to assume little endian byte order */
    if (!vcbBigEndian()) {
		write_short = write_short_normal;
        write_int = write_int_normal;
        write_long = write_long_normal;
    } 
	else 
	{
        write_short = write_short_reverse;
        write_int = write_int_reverse;
        write_long = write_long_reverse;
    }

	code = (2 << 16);
	write_int(fp, code);
	code = 0;
	write_int(fp, code);
	write_int(fp, code);
	write_short(fp, (short)width);
	write_short(fp, (short)height);
	bits = 24;
	fwrite(&bits, sizeof(char), 1, fp);
	bits = 0;
	fwrite(&bits, sizeof(char), 1, fp);

	/* store all pixel data in byte format */
	npixels = width * height;
	for (i = 0; i < npixels; i ++) {
		fwrite(&fbuffer[i*nchannels + 2], sizeof(unsigned char), 1, fp);
		fwrite(&fbuffer[i*nchannels + 1], sizeof(unsigned char), 1, fp);
		fwrite(&fbuffer[i*nchannels + 0], sizeof(unsigned char), 1, fp);
	}

	fclose(fp);
}

void vcbImgWriteBMP(char *filename, unsigned char * fbuffer, int nchannels, int width, int height)
{
    FILE * fp;
    int i, j, k, npixels;
    int row, column, nbytes_padding;
    int offset = 11 * 4 + 5 * 2;
    int size = height * width * nchannels + offset;
    void (* write_short) (FILE *, short);
    void (* write_int) (FILE *, int);
    void (* write_long) (FILE *, long);

    /* write bitmap to a file */
    if (nchannels < 3) {
       fprintf(stderr,"BMP requires at lease 3 color channels, not %d (input)\n",nchannels);
       return;
    }
  
    if((fp=fopen(filename,"wb")) == NULL) {
       perror(filename);     
       return;
    }

    /* since bmp is a microsoft format, it assumes little endian byte order */
    if (!vcbBigEndian()) {
       write_short = write_short_normal;
       write_int = write_int_normal;
       write_long = write_long_normal;
    } 
    else 
    {
       write_short = write_short_reverse;
       write_int = write_int_reverse;
       write_long = write_long_reverse;
    }

    /* Scanlines need to be padded to multiples of 4 in BMP.  Take
     * this into account when determining file size. */
    nbytes_padding = (4 - (width * nchannels) % 4)% 4;
    size += nbytes_padding * height;

    /* output header */
    write_short (fp, 19778);
    write_int (fp, size);
    write_short (fp, 0);
    write_short (fp, 0);
    write_int (fp, offset);
    write_int (fp, 9 * 4 + 2 * 2);
    write_int (fp, width);
    write_int (fp, height);
    write_short (fp, 1);
    write_short (fp, 24);
    write_int (fp, 0);
    write_int (fp, 0);
    write_int (fp, 0);
    write_int (fp, 0);
    write_int (fp, 0);
    write_int (fp, 0);

    /* Write blue, green, red. */
    for (i = 0; i < height; i++) {
       row = i * width * nchannels;
       for (j = 0; j < width; j++) {
          column = j * nchannels;
          for (k = nchannels - 1; k >= 0; k--) {
             fwrite(&(fbuffer[row + column + k]), sizeof(unsigned char), 1, fp);
          }
       }

       /* Added to pad out rows when they're not multiples of 4. */
       for (k = 0; k < nbytes_padding; k++) {
          fputc(0, fp);
       }

    }

    fclose (fp);
}

void vcbImgWriteRGB(char *filename, unsigned char * fbuffer, int nchannels, int width, int height)
{
	FILE * fp;
    int i, j, npixels;
	char imgname[80];
    void (* write_short) (FILE *, short);
    void (* write_int) (FILE *, int);
    void (* write_long) (FILE *, long);

   	if (nchannels < 3) {
		fprintf(stderr,"RGB requires at lease 3 color channels, not %d (input)\n",nchannels);
		return;
	}
  
	if((fp=fopen(filename,"w")) == NULL) {
		perror(filename);     
		return;
    }

	/* since bmp is a microsoft format, it assumes little endian byte order */
    if (!vcbBigEndian()) {
		write_short = write_short_normal;
        write_int = write_int_normal;
        write_long = write_long_normal;
    } 
	else 
	{
        write_short = write_short_reverse;
        write_int = write_int_reverse;
        write_long = write_long_reverse;
    }

	/* output header */   
	write_short (fp, 474);                        /* magic number */
	write_char  (fp, 0);                          /* storage, uncompressed */
	write_char  (fp, 1);                          /* bytes per channel */
	write_short (fp, 3);                          

	write_short (fp, (short) width); 
	write_short (fp, (short) height);
	write_short (fp, (short) 3);                  /* always do 3 channels */
	write_long  (fp, 0);                          /* minimum pixel value */
	write_long  (fp, 255);                        /* maximum pixel value */

	for (i = 1; i <= 4; i++)                      /* four dummy bytes */
		write_char (fp, '\0');
   
	sprintf(imgname,"%s",filename);
	fwrite (imgname, sizeof (char), 80, fp);      /* image name */
	write_long (fp, 0);                           /* colormap */
	for (i = 1; i <= 404; i++)                    /* 404 dummy bytes */
		write_char (fp, '\0');

    /* write red channel first, then green, then blue */
	npixels = width * height;
	for (j = 0; j < 3; j ++)
		for (i = 0; i < npixels; i ++)
			fwrite(&fbuffer[i*nchannels + j], sizeof(unsigned char), 1, fp);

    fclose (fp);
}


unsigned char * vcbImgReadPPM(char *filename, int * width, int * height)
{
  FILE *fp;
  unsigned char * fbuffer;
  int npixels, nread;
  char errormsg[80];

  if((fp=fopen(filename,"rb")) == NULL) {
	  perror(filename);
	  return NULL;
  }

  npixels = fscanf(fp,"P6\n%d %d\n255", width, height);
  printf("reading %d bytes so far\n, fseek = %d", npixels, ftell(fp));

  npixels = (*width)*(*height);
  fbuffer = (unsigned char *) malloc(npixels*sizeof(unsigned char)*3);
  if (fbuffer == NULL) {
	  perror("Cannot allocate memory in vcbImgReadPPM");
      fclose(fp); 
	  return NULL;
  }

  for (npixels *= 3, nread = 0; (npixels > 0) && (nread > -1); npixels -= nread) {
    nread = (int) fread(fbuffer,sizeof(unsigned char),npixels,fp);
	printf("nread = %d, npixels = %d\n", nread, npixels);
  }
  
  /*if (nread < npixels *3) { */
  if (npixels > 0) {
	  sprintf(errormsg,"vcbImageRead failed, only read in %d pixels in %s",npixels,filename);
	  free(fbuffer);
	  perror(errormsg);
      fclose(fp); 
	  return NULL;
  }

  fclose(fp); 
  return fbuffer;
}

unsigned char * vcbImgReadPGM(char *filename, int * width, int * height)
{
  FILE *fp;
  unsigned char * fbuffer;
  int npixels, nread;
  char errormsg[80];

  if((fp=fopen(filename,"r")) == NULL) {
	  perror(filename);
	  return NULL;
  }

  fscanf(fp,"P5\n%d %d\n255\n", width, height);
  npixels = (*width)*(*height);
  fbuffer = (unsigned char *) malloc(npixels*sizeof(unsigned char));
  if (fbuffer == NULL) {
	  perror("cannot allocate memory in vcbImgReadPPM");
      fclose(fp); 
	  return NULL;
  }

  nread = (int) fread(fbuffer,sizeof(unsigned char),npixels,fp);
  if (nread < npixels) {
	  sprintf(errormsg,"failed, only read in %d pixels in %s",nread,filename);
	  free(fbuffer);
	  perror(errormsg);
      fclose(fp); 
	  return NULL;
  }

  fclose(fp); 
  return fbuffer;
}
