/***************************************************************************
 *   Copyright (C) 2004  Jian Huang                                        *
 *   seelab@cs.utk.edu                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "vcbutils.h"

/* an internal convenient macro */
#ifndef vcb_talloc
#define vcb_talloc(ty, sz) (ty *) malloc (sz*sizeof(ty))
#endif

/* vcb data formats */
static char vcbdataformats [9][16] = {"unsigned byte",
                      "byte",
                      "unsigned short",
                      "short",
                      "unsigned int",
                      "int",
                      "float",
                      "double",
                      "bitfield"};

char * vcbStrDataType(vcbdatatype d)
{
  return vcbdataformats[d];
}

/*
 * ========================================
 * vcb functions to read/write binary files 
 * ========================================
 */

int vcbBigEndian (void) 
/* 
 * this function tests whether the host machine is big/little endian
 * return value: 1 - big endian, 0 - little endian
 */
{
   int test = 1;

   if ( (((char *)(&test))[0]) == 1) {
      return 0;
   } else {
      return 1;
   }
}

void vcbToggleEndian(void * dptr, int nbytes)
/* toggle the byte order in the nbytes pointed to by dptr */
{
    char * sptr = (char *) dptr;
    char * eptr = (char *) dptr + (nbytes - 1);
    char tmp;

    while (sptr < eptr) {
        tmp = *sptr;
        *sptr = *eptr;
        *eptr = tmp;
        sptr ++;
        eptr --;
    }
}

int     vcbSizeOf(vcbdatatype vcbtype)
/* return the size of a vcbtype */
{
    int numbytes;

    switch (vcbtype) {
        case VCB_UNSIGNEDBYTE: case VCB_BYTE:
            numbytes = sizeof(char);   break;
        case VCB_UNSIGNEDSHORT: case VCB_SHORT:
            numbytes = sizeof(short);  break;
        case VCB_UNSIGNEDINT: case VCB_INT: case VCB_BITFIELD:
            numbytes = sizeof(int);    break;
        case VCB_FLOAT:
            numbytes = sizeof(float);  break;
        case VCB_DOUBLE:
            numbytes = sizeof(double); break;
        default:
            numbytes = 1;              break;
    }

    return numbytes;
}

void vcbReadHeaderBinf(char * filename, int endian, int * dims, int * dsize, int * nattribs)
/*
 * filename:    name of file to read, the full name including extension
 * endian:      the endian order on the current machine (little/big endian)
 * dims:        number of dimensions
 * dsize:       the number of voxels in each dimension, in an array allocated outsize this function
 * nattribs:    number of attibutes on each voxel.
 *
 * return:      -
 */
{
#ifndef VCB_USECUR_ENDIAN
#define VCB_USECUR_ENDIAN 0
#endif
#ifndef VCB_USEINV_ENDIAN
#define VCB_USEINV_ENDIAN 1
#endif

    int i;
    int change_endian;
    unsigned char file_endian;
    char errormsg [100];
    FILE * datain;

    datain = fopen(filename,"rb");
    if (datain == NULL) {
        sprintf(errormsg,"cannot open %s to read\n", filename);
        perror(errormsg);
        return;
    }

    fread(&file_endian, sizeof(unsigned char), 1, datain);
    change_endian = (endian == file_endian) ? VCB_USECUR_ENDIAN : VCB_USEINV_ENDIAN;

    fread(dims, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        vcbToggleEndian(dims,sizeof(int));

    fread(dsize, sizeof(int), *dims, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        for (i = 0; i < *dims; vcbToggleEndian(&dsize[i], sizeof(int)), i++)
            ;

    fread(nattribs, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN) vcbToggleEndian(nattribs, sizeof(int));

    return;
}

int vcbReadHeaderBinm(char * filename, vcbdatatype * file_datatype, int * dims, int * orig, int * dsize, int * nattribs)
/*
 * filename:    name of file to read, the full name including extension
 * endian:      the endian order on the current machine (little/big endian)
 * dims:        number of dimensions
 * dsize:       the number of voxels in each dimension, in an array allocated outsize this function
 * nattribs:    number of attibutes on each voxel.
 *
 * return:      0 - success, -1 - failure
 */
{
#ifndef VCB_USECUR_ENDIAN
#define VCB_USECUR_ENDIAN 0
#endif
#ifndef VCB_USEINV_ENDIAN
#define VCB_USEINV_ENDIAN 1
#endif

    int i;
    int file_endian = 1, change_endian;
    char errormsg [100];
    FILE * datain;

    datain = fopen(filename,"rb");
    if (datain == NULL) {
        sprintf(errormsg,"cannot open %s to read\n", filename);
        perror(errormsg);
        return -1;
    }

    fread(&file_endian, sizeof(int), 1, datain);
    change_endian = (1 == file_endian) ? VCB_USECUR_ENDIAN : VCB_USEINV_ENDIAN;

    fread(file_datatype, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        vcbToggleEndian(file_datatype,sizeof(int));

    fread(dims, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        vcbToggleEndian(dims,sizeof(int));

    fread(orig, sizeof(int), *dims, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        for (i = 0; i < *dims; vcbToggleEndian(&orig[i], sizeof(int)), i++);

    fread(dsize, sizeof(int), *dims, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        for (i = 0; i < *dims; vcbToggleEndian(&dsize[i], sizeof(int)), i++);

    fread(nattribs, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN) vcbToggleEndian(nattribs, sizeof(int));

    fclose(datain);
    return 0;
}


#ifndef VCB_USECUR_ENDIAN
#define VCB_USECUR_ENDIAN 0
#endif
#ifndef VCB_USEINV_ENDIAN
#define VCB_USEINV_ENDIAN 1
#endif


void * vcbReadBinf(char * filename, vcbdatatype rawdatatype, int endian, int * dims, int * dsize, int * nattribs)
/*
 * filename:    name of file to read, the full name including extension
 * rawdatatype: the type of binary data in the file. the first numbers are always
 *              endian, dims, dsize[0] ... dsize[dims-1], nattribs.
 *              raw data follows this list
 * endian:      the endian order on the current machine (little/big endian)
 * dims:        number of dimensions
 * dsize:       the number of voxels in each dimension, in an array allocated outsize this function
 * nattribs:    number of attibutes on each voxel. (all attributes are of the rawdatatype type)
 *
 * return:      NULL for failure, the actually pointer to the firstbyte of the linear array
 *              vcbReadBinf allocates memory for this array using malloc
 */
{

    int nvals, nbytes, nread, i;
    int change_endian;
    unsigned char file_endian;
    char errormsg [100];
    FILE * datain;
    void * rawdata;

    datain = fopen(filename,"rb");
    if (datain == NULL) {
        sprintf(errormsg,"cannot open %s to read\n", filename);
        perror(errormsg);
        return NULL;
    }

    fread(&file_endian, sizeof(unsigned char), 1, datain);
    change_endian = (endian == file_endian) ? VCB_USECUR_ENDIAN : VCB_USEINV_ENDIAN;

    fread(dims, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        vcbToggleEndian(dims,sizeof(int));

    fread(dsize, sizeof(int), *dims, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        for (i = 0; i < *dims; vcbToggleEndian(&dsize[i], sizeof(int)), i++)
            ;

    fread(nattribs, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN) vcbToggleEndian(nattribs, sizeof(int));

    /*printf("reading %s expecting %d dimensions, %d by %d by %d voxels, %d attributes per voxel\n", \
        filename, *dims,dsize[0],dsize[1],dsize[2],*nattribs);*/

    nvals    = dsize[0]*dsize[1]*dsize[2]*nattribs[0];
    nbytes   = nvals * vcbSizeOf(rawdatatype);
    rawdata  = malloc (nbytes);

    if (rawdata == NULL) {
        sprintf(errormsg,"cannot allocate space for rawdata array, need %d bytes\n", nbytes);
        perror(errormsg);
        fclose(datain);
        return NULL;
    }

    nread = (int) fread(rawdata, vcbSizeOf(rawdatatype), nvals, datain);
    if ( nread < nvals) {
        sprintf(errormsg,"In %s, can only read %d vals out of %d\n", filename, nread, nvals);
        perror(errormsg);
        free(rawdata);
        fclose(datain);
        return NULL;
    }

    fclose(datain);
    return rawdata;
}

void * vcbReadBinm(char * filename, vcbdatatype * file_datatype, int * dims, int * orig, int * dsize, int * nattribs)
/*
 * filename:    name of file to read, the full name including extension
 * rawdatatype: the type of binary data in the file. the first numbers are always
 *              endian, dims, dsize[0] ... dsize[dims-1], nattribs.
 *              raw data follows this list
 * endian:      the endian order on the current machine (little/big endian)
 * dims:        number of dimensions
 * dsize:       the number of voxels in each dimension, in an array allocated outsize this function
 * nattribs:    number of attibutes on each voxel. (all attributes are of the rawdatatype type)
 *
 * return:      NULL for failure, the actually pointer to the firstbyte of the linear array
 *              vcbReadBinf allocates memory for this array using malloc
 */
{

    int nvals, nbytes, nread, i;
    int file_endian = 1, change_endian;
    char errormsg [100];
    FILE * datain;
    void * rawdata;

    datain = fopen(filename,"rb");
    if (datain == NULL) {
        sprintf(errormsg,"cannot open %s to read\n", filename);
        perror(errormsg);
        return NULL;
    }

    fread(&file_endian, sizeof(int), 1, datain);
    change_endian = (1 == file_endian) ? VCB_USECUR_ENDIAN : VCB_USEINV_ENDIAN;

    
    fread(file_datatype, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        vcbToggleEndian(file_datatype,sizeof(int));
    /*
    if (file_datatype != rawdatatype) {
      fprintf(stderr,"vcbReadBinm: expected datatype does not match file\n");
      fclose(datain);
      return NULL;
    }
    */

    fread(dims, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        vcbToggleEndian(dims,sizeof(int));

    fread(orig, sizeof(int), *dims, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        for (i = 0; i < *dims; vcbToggleEndian(&orig[i], sizeof(int)), i++);

    fread(dsize, sizeof(int), *dims, datain);
    if (change_endian == VCB_USEINV_ENDIAN)
        for (i = 0; i < *dims; vcbToggleEndian(&dsize[i], sizeof(int)), i++);

    fread(nattribs, sizeof(int), 1, datain);
    if (change_endian == VCB_USEINV_ENDIAN) vcbToggleEndian(nattribs, sizeof(int));

    /*printf("reading %s expecting %d dimensions, %d by %d by %d voxels, %d attributes per voxel\n", \
        filename, *dims,dsize[0],dsize[1],dsize[2],*nattribs);*/

    for (i = 0, nvals = *nattribs; i < *dims; i++)
      nvals *= dsize[i];

    nbytes   = vcbSizeOf(*file_datatype);
    rawdata  = malloc (nvals * nbytes);

    if (rawdata == NULL) {
        sprintf(errormsg,"cannot allocate space for rawdata array, need %d bytes\n", nbytes);
        perror(errormsg);
        fclose(datain);
        return NULL;
    }

    nread = (int) fread(rawdata, nbytes, nvals, datain);
    if ( nread < nvals) {
        sprintf(errormsg,"In %s, can only read %d vals out of %d\n", filename, nread, nvals);
        perror(errormsg);
        free(rawdata);
        fclose(datain);
        return NULL;
    }

    fclose(datain);

    if (change_endian == VCB_USEINV_ENDIAN) {
      char * cp = rawdata;
      for (i = 0; i < nvals; cp += nbytes, i ++)
        vcbToggleEndian(cp, nbytes);
    }

    return rawdata;
}


void * vcbLoadBinm(void * raw, vcbdatatype * rawdatatype, int * dims, int * orig, int * dsize, int * nattribs)
/*
 * reading binary data from a mmap'ed file
 *
 * raw:         void * pointer to byte 0 of mmap'ed file
 * rawdatatype: the type of binary data in the file. the first numbers are always
 *              endian, dims, dsize[0] ... dsize[dims-1], nattribs.
 *              raw data follows this list
 * dims:        number of dimensions
 * dsize:       the number of voxels in each dimension, in an array allocated outsize this function
 * nattribs:    number of attibutes on each voxel. (all attributes are of the rawdatatype type)
 *
 * return:      NULL for failure, the actually pointer to the firstbyte of the linear array
 *              of all voxels. Since it is mmap'ed, no malloc call is necessary
 */
{
    int nvals, nbytes, i;
    int endian, change_endian;
    int * vp;
    char * cp;
    void * rawdata;

    vp = (int *) raw;
    endian = 1;

    change_endian = (endian == *vp) ? VCB_USECUR_ENDIAN : VCB_USEINV_ENDIAN;
    (*vp) = endian;
    vp ++;

    /*
    if (change_endian == VCB_USEINV_ENDIAN)
      printf("need to change endian\n");
    */

        if (change_endian == VCB_USEINV_ENDIAN)
          vcbToggleEndian(vp,sizeof(int));
    memcpy(rawdatatype, vp, sizeof(int));
    vp ++;

    if (change_endian == VCB_USEINV_ENDIAN)
      vcbToggleEndian(vp,sizeof(int));
    memcpy(dims,vp, sizeof(int));
    vp ++;

    if (change_endian == VCB_USEINV_ENDIAN) 
      for (i = 0; i < *dims; vcbToggleEndian(&vp[i], sizeof(int)), i++);
    memcpy(orig, vp, sizeof(int)*(*dims));
    vp += *dims;

    if (change_endian == VCB_USEINV_ENDIAN) 
      for (i = 0; i < *dims; vcbToggleEndian(&vp[i], sizeof(int)), i++);
    memcpy(dsize, vp, sizeof(int)*(*dims));
    vp += *dims;

    if (change_endian == VCB_USEINV_ENDIAN)
      vcbToggleEndian(vp, sizeof(int));
    memcpy(nattribs,vp,sizeof(int));
    vp ++;

    /*printf("reading mmap file expecting %d dimensions, %d by %d by %d voxels, %d attributes per voxel\n", *dims,dsize[0],dsize[1],dsize[2],*nattribs);*/

    for (i = 0, nvals = *nattribs; i < *dims; i++)
      nvals *= dsize[i];

    nbytes   = vcbSizeOf(*rawdatatype);

    /*printf("nvals = %d, nbytes = %d\n",nvals, nbytes);*/

    rawdata  = vp;

    if (change_endian == VCB_USEINV_ENDIAN) {
      for (i = 0, cp = (char*)vp; i < nvals; cp += nbytes, i ++)
        vcbToggleEndian(cp, nbytes);
    }

    return rawdata;
}

int vcbWriteBinf(char * filename, vcbdatatype rawdatatype, int endian, int dims, int * dsize, int nattribs, void * rawdata)
/*
 * filename:    name of file to write, the name WITHOUT extension (.bin.xx will be added to the name automatically)
 * rawdatatype: the type of binary data in the file. the first numbers are always
 *              endian, dims, dsize[0] ... dsize[dims-1], nattribs.
 *              raw data follows this list
 * endian:      the endian order on the current machine (little/big endian)
 * dims:        number of dimensions
 * dsize:       the number of voxels in each dimension, in an array
 * nattribs:    number of attibutes on each voxel. (all attributes are of the rawdatatype type)
 * rawdata:     pointer to the array of raw data
 *
 * return:      0 for failure, 1 for success
 *
 */
{
    int nvals, nwrote;
    char errormsg [100];
    char filenamebin [100];
    char datatypeend [100];
    FILE * dataout;

    switch (rawdatatype) {
    case VCB_UNSIGNEDBYTE:
        sprintf(datatypeend,"ub");
        break;
    case VCB_BYTE:
        sprintf(datatypeend,"b");
        break;
    case VCB_UNSIGNEDSHORT:
        sprintf(datatypeend,"us");
        break;
    case VCB_SHORT:
        sprintf(datatypeend,"s");
        break;
    case VCB_UNSIGNEDINT:
        sprintf(datatypeend,"ui");
        break;
    case VCB_INT:
        sprintf(datatypeend,"i");
        break;
    case VCB_FLOAT:
        sprintf(datatypeend,"f");
        break;
    case VCB_DOUBLE:
        sprintf(datatypeend,"d");
        break;
    case VCB_BITFIELD:
        sprintf(datatypeend,"bf");
        break;
    }

    sprintf(filenamebin, "%s.%s", filename, datatypeend);

    dataout = fopen(filenamebin,"wb");
    if (dataout == NULL) {
        sprintf(errormsg,"cannot open %s to write\n", filenamebin);
        perror(errormsg);
        return 0;
    }

    fwrite(&endian, sizeof(unsigned char), 1, dataout);    
    fwrite(&dims, sizeof(int), 1, dataout);     
    fwrite(dsize, sizeof(int), dims, dataout);
    fwrite(&nattribs, sizeof(int), 1, dataout);
    nvals = dsize[0]*dsize[1]*dsize[2]*nattribs;

    nwrote = (int) fwrite(rawdata, vcbSizeOf(rawdatatype), nvals, dataout);
    if ( nwrote < nvals) {
        sprintf(errormsg,"In %s, can only write %d vals out of %d\n", filename, nwrote, nvals);
        perror(errormsg);
        fclose(dataout);
        return 0;
    }

    fclose(dataout);
    return 1;
}

int vcbGenBinm(char * filename, vcbdatatype rawdatatype, int dims, int * orig, int * dsize, int nattribs, void * rawdata)
/*
 * filename:    name of file to write, the name WITHOUT extension (.bin.xx will be added to the name automatically)
 * rawdatatype: the type of binary data in the file. the first numbers are always
 *              endian, dims, dsize[0] ... dsize[dims-1], nattribs.
 *              raw data follows this list
 * endian:      the endian order on the current machine (little/big endian)
 * dims:        number of dimensions
 * dsize:       the number of voxels in each dimension, in an array
 * nattribs:    number of attibutes on each voxel. (all attributes are of the rawdatatype type)
 * rawdata:     pointer to the array of raw data
 *
 * return:      1 for failure, 0 for success
 *
 */
{
        int nvals, nwrote;
    int endian = 1;
        char errormsg [100];
        char filenamebin [100];
        char datatypeend [100];
        FILE * dataout;
    int i;

        switch (rawdatatype) {
        case VCB_UNSIGNEDBYTE:
                sprintf(datatypeend,"ub");
                break;
        case VCB_BYTE:
                sprintf(datatypeend,"b");
                break;
        case VCB_UNSIGNEDSHORT:
                sprintf(datatypeend,"us");
                break;
        case VCB_SHORT:
                sprintf(datatypeend,"s");
                break;
        case VCB_UNSIGNEDINT:
                sprintf(datatypeend,"ui");
                break;
        case VCB_INT:
                sprintf(datatypeend,"i");
                break;
        case VCB_FLOAT:
                sprintf(datatypeend,"f");
                break;
        case VCB_DOUBLE:
                sprintf(datatypeend,"d");
                break;
        case VCB_BITFIELD:
                sprintf(datatypeend,"bf");
                break;
        }

        sprintf(filenamebin, "%s.%s", filename, datatypeend);

        dataout = fopen(filenamebin,"wb");
        if (dataout == NULL) {
      sprintf(errormsg,"cannot open %s to write\n", filenamebin);
      perror(errormsg);
      return 1;
        }

        fwrite(&endian,      sizeof(int), 1,    dataout);
    fwrite(&rawdatatype, sizeof(int), 1,    dataout);
        fwrite(&dims,        sizeof(int), 1,    dataout);
    fwrite(orig,         sizeof(int), dims, dataout);
        fwrite(dsize,        sizeof(int), dims, dataout);
        fwrite(&nattribs,    sizeof(int), 1,    dataout);
    
    for (i = 0, nvals = nattribs; i < dims; i++)
      nvals *= dsize[i];

        nwrote = (int) fwrite(rawdata, vcbSizeOf(rawdatatype), nvals, dataout);
        if ( nwrote < nvals) {
      sprintf(errormsg,"In %s, can only write %d vals out of %d\n", filename, nwrote, nvals);
      perror(errormsg);
      fclose(dataout);
      return 1;
        }

        fclose(dataout);
        return 0;
}


int vcbMakeBinm(char * output, vcbdatatype rawdatatype, int dims, int * orig, int * dsize, int nattribs, void * rawdata)
/*
 * filename:    name of file to write, the name WITHOUT extension (.bin.xx will be added to the name automatically)
 * rawdatatype: the type of binary data in the file. the first numbers are always
 *              endian, dims, dsize[0] ... dsize[dims-1], nattribs.
 *              raw data follows this list
 * endian:      the endian order on the current machine (little/big endian)
 * dims:        number of dimensions
 * dsize:       the number of voxels in each dimension, in an array
 * nattribs:    number of attibutes on each voxel. (all attributes are of the rawdatatype type)
 * rawdata:     pointer to the array of raw data
 *
 * return:      1 for failure, 0 for success
 *
 */
{
    int nvals;
    int endian = 1;
    int i;

    if (output == NULL) {
      fprintf(stderr,"vcbMakeBinm failed due to NULL output ptr\n");
      return 1;
    }

    memcpy(output, &endian, sizeof(int)); 
    output += sizeof(int);
    memcpy(output, &rawdatatype, sizeof(int)); 
    output += sizeof(int);
    memcpy(output, &dims, sizeof(int));
    output += sizeof(int);
    memcpy(output, orig, sizeof(int)*dims);
    output += sizeof(int)*dims;
    memcpy(output, dsize, sizeof(int)*dims);
    output += sizeof(int)*dims;
    memcpy(output, &nattribs, sizeof(int));
    output += sizeof(int);
    
    for (i = 0, nvals = nattribs; i < dims; i++)
      nvals *= dsize[i];

    memcpy(output, rawdata, vcbSizeOf(rawdatatype)*nvals);

        return 0;
}


/*
 * =========================================
 * VR data struct functions
 * =========================================
 */

char * alloc_init_bytes(vcbdatatype vcbtype, int numele, void * target)
/* 
 * mostly an internal function for allocating memory for numele
 * elements of the given type. if a target is provided, initiate
 * the array to this target too, else clear every bit to zero 
 */
{
    char * dest;
    int numbytes = vcbSizeOf(vcbtype) * numele;

    dest = vcb_talloc(char, numbytes);
    if (dest == NULL) {
        fprintf(stderr,"cannot allocate memory, %d bytes requested\n", numbytes);
        return NULL;
    }

    if (target == NULL) 
        memset(dest, '\0', numbytes);   /* clear every bit to zero */
    else
        memcpy(dest, target, numbytes); /* direct copy of every bit in attribs */

    return dest;
}

VCBdata1d new_data1dstruct (vcbdatatype vcbtype, int isizeDim, int numAttribs, void * attribs)
/* 
 * if attribs == NULL, then assuming no definitive data is provided.
 * isizeDim - integer describing the size of the first (the only) dimension
 * numAttribs - number of attributes per data point
 * return NULL when fail and a pointer if successful
 */
{
    int numele;
    VCBdata1d d1;

    if (isizeDim <= 0){
        fprintf(stderr,"cannot allocate %d size vrdata21 struct",isizeDim);
        return NULL;
    }

    if (vcbtype > VCB_BITFIELD || vcbtype < VCB_UNSIGNEDBYTE) {
        fprintf(stderr,"type == %d is unknown for vcbdatatype",(int)vcbtype);
        return NULL;
    }

    d1 =  vcb_talloc(struct data1dStruct, 1);
    d1 -> type = vcbtype;
    d1 -> isize1stDim = isizeDim;
    d1 -> inumAttribs = numAttribs;

    numele = isizeDim*numAttribs;

      d1 -> v_list = (void *) alloc_init_bytes(vcbtype, numele, attribs);
    return d1;
}

void  free_data1dstruct(VCBdata1d vcbdata1d)
{
    if (vcbdata1d) {
        if (vcbdata1d -> v_list)
            free(vcbdata1d -> v_list);

        free(vcbdata1d);
    }
}

VCBdata2d new_data2dstruct (vcbdatatype vcbtype, int isize1stDim, int isize2ndDim, int numAttribs, void * attribs)
{
    int numele;
    VCBdata2d d2;

    if (isize1stDim <= 0 || isize2ndDim <= 0) {
        fprintf(stderr,"cannot allocate %d by %d vcbdata2d struct",isize1stDim,isize2ndDim);
        return NULL;
    }

    if (vcbtype > VCB_BITFIELD || vcbtype < VCB_UNSIGNEDBYTE) {
        fprintf(stderr,"type == %d is unknown for vcbdatatype",(int)vcbtype);
        return NULL;
    }

    d2 =  vcb_talloc(struct data2dStruct, 1);
    d2 -> type = vcbtype;
    d2 -> isize1stDim = isize1stDim;
    d2 -> isize2ndDim = isize2ndDim;
    d2 -> inumAttribs = numAttribs;

    d2 -> fscale1stDim = 1.0f; /* default */
    d2 -> fscale2ndDim = 1.0f; /* default */

    numele = isize1stDim*isize2ndDim*numAttribs;

    d2 -> v_list = (void *) alloc_init_bytes(vcbtype, numele, attribs);
    return d2;
}

void set_data2dscaling(VCBdata2d vcbdata2d, float fscale1stDim, float fscale2ndDim)
{
    vcbdata2d -> fscale1stDim = fscale1stDim;
    vcbdata2d -> fscale2ndDim = fscale2ndDim;
}

void free_data2dstruct(VCBdata2d vcbdata2d)
{
    if (vcbdata2d) {
        if (vcbdata2d -> v_list)
            free(vcbdata2d -> v_list);

        free(vcbdata2d);
    }    
}

VCBdata3d new_data3dstruct (vcbdatatype vcbtype, int isize1stDim, int isize2ndDim, int isize3rdDim, int numAttribs, void * attribs)
{
    int numele;
    VCBdata3d d3;

    if (isize1stDim <= 0 || isize2ndDim <= 0 || isize3rdDim <= 0) {
        fprintf(stderr,"cannot allocate %d by %d vcbdata2d struct",isize1stDim,isize2ndDim);
        return NULL;
    }

    if (vcbtype > VCB_BITFIELD || vcbtype < VCB_UNSIGNEDBYTE) {
        fprintf(stderr,"type == %d is unknown for vcbdatatype",(int)vcbtype);
        return NULL;
    }

    d3 =  vcb_talloc(struct data3dStruct, 1);
    d3 -> type = vcbtype;
    d3 -> isize1stDim = isize1stDim;
    d3 -> isize2ndDim = isize2ndDim;
    d3 -> isize3rdDim = isize3rdDim;
    d3 -> inumAttribs = numAttribs;

    d3 -> fscale1stDim = 1.0f; /* default */
    d3 -> fscale2ndDim = 1.0f;
    d3 -> fscale3rdDim = 1.0f;

    numele = isize1stDim*isize2ndDim*isize3rdDim*numAttribs;

    d3 -> v_list = (void *) alloc_init_bytes(vcbtype, numele, attribs);
    return d3;
}

void  set_data3dscaling(VCBdata3d vcbdata3d, float fscale1stDim, float fscale2ndDim, float fscale3rdDim)
{
    vcbdata3d -> fscale1stDim = fscale1stDim;
    vcbdata3d -> fscale2ndDim = fscale2ndDim;
    vcbdata3d -> fscale3rdDim = fscale3rdDim;
}

void  free_data3dstruct(VCBdata3d vcbdata3d)
{
    if (vcbdata3d) {
        if (vcbdata3d -> v_list)
            free(vcbdata3d -> v_list);

        free(vcbdata3d);
    }
}


/* 
 * ============================================
 * vcbGrablk function and all it's modules 
 * ============================================
 */

static int compare( const void *arg1, const void *arg2 )
/* internal function for use with qsort */
{
    VCBpqe * pqe1, * pqe2;
    pqe1 = (VCBpqe *) arg1;
    pqe2 = (VCBpqe *) arg2;

    if (pqe1->priority <  pqe2->priority) return -1;
    if (pqe1->priority >  pqe2->priority) return 1;

    return 0;
}

int vcbDefaultIndex(int ndims, int * dsizes, int * coords)
/* VCB default indexing func, coords values start from zero */
{
    int i, id = coords[0];
    for (i = 1; i < ndims; i ++)
        id = id*dsizes[i] + coords[i];
    return id;
}

void vcbDefaultIndexToCoords(int ndims, int *dims, int index, int *coords) {

   /* Map an index back to its coordinates. */

   int j;
   int divisor = 1;

   for (j = ndims - 1; j >= 0; j--) {
      /* Strip off the previous dimensions' layers and wrap along current
       * layer to find the voxel's position in current dimension. */
      coords[j] = (index / divisor) % dims[j];

      /* Add current dimension's layer of voxels. */
      divisor *= dims[j];
   }

}

static int incr_indx(int * clbounds, int ndims, int * lbounds, int * ubounds, VCBpqe * fastindex)
/* internal function
 * return value: 0 for normal incr
 *               1 for overflow
 */
{
    int i = 1;

    if (ndims  < 2) /* already overflowed */
        return 1;

    clbounds[fastindex[i].id] ++;
    while ((clbounds[fastindex[i].id] > ubounds[fastindex[i].id]) && (i < ndims)){
        clbounds[fastindex[i].id] = lbounds[fastindex[i].id];
        i ++;
        if (i < ndims)
            clbounds[fastindex[i].id] ++;
        else
            break;
    }

    return (i < ndims) ? 0 : 1;
}

int vcbGrablk(void * bindata, void * boutdata,
              int nattribs, int valsize,      
              int ndims, int * dsizes,      
              int * lbounds, int * ubounds, 
              int (* indexfunc)(int, int *, int *))
/* 
 * grablk stands for grab block.
 *
 * given a ndims dimension raw binary data pointed to by bindata, with nattribs attributs on each 
 * data element, this call grabs the block described by lowerbounds (lbounds) and upperbounds (ubounds)
 * specified in each dimension. The number of bytes of each value in the binary data is valsize.
 *
 * be inclusive of lower and upper bounds when reading
 *
 * no clipping is done. grablk always assumes a malloc'ed buffer of the size
 * specified by the bounds. grablk fills zeros in the space outside bindata's domain
 *
 * boutdata is a pointer to memory already allocated to hold the grabbed block
 *
 * return value: -1 for failure, >= 0: the actual number of data elements grabed from bindata
 *
 * if you do not index your volume using the VR convention, please provide a indexfunction.
 * otherwise, simply give NULL for indexfunc. the default index function used is as following:
 *
 *    int vcbDefaultIndex(int ndims, int * dsizes, int * coords)
 *    {
 *          int i, id = coords[0];
 *            for (i = 1; i < ndims; i ++)
 *                id = id*dsizes[i] + coords[i];
 *
 *            return id;
 *    }
 *
 * if using a different indexing scheme, an indexing function using this exact interface 
 * must be provided.
 */
{
    int i, j;
    int onestep[10];                /* assuming at most do ten dimension datasets */
    VCBpqe fastrank[10];             /* how fast does each axis vary, slowest -> 1 */
    int elesize;                    /* size of each data element, in bytes */
    int clbounds[10];

    /* these following locations are measured in number of data elements, like pixel, voxel */
    int first, last;                /* location of 1st and last read */
    int gstride, sstride;           /* stride to grab, step in block */
    int blksize;                    /* size of block to read, consider clipping */

    char * blkptr = NULL;
    char * indata;

    VCB_indexfunc ifunc;

    ifunc = vcbDefaultIndex;
    if (indexfunc != NULL) ifunc = indexfunc;

    if (bindata == NULL) {
        fprintf(stderr,"grab_block got given NULL bindata to grab from\n");
        return -1;
    }

    if (ndims < 1) {
        fprintf(stderr,"grab_block got %d dimension bindata to grab from, don't know how to handle\n", ndims);
        return -1;
    }

    for (i = 0; i < ndims; i ++) {
        if (dsizes[i] < 0) {
            fprintf(stderr,"grab_block got negative bindata dimension size, dim_%d = %d\n",i,dsizes[i]);
            return -1;
        }

        if (lbounds[i] > ubounds[i]) {
            fprintf(stderr,"grab_block got improper dim_%d bounds (lower, upper) = (%d,%d)\n",i,lbounds[i],ubounds[i]);
            return -1;
        }

        if ((ubounds[i] < 0) || (lbounds[i] > dsizes[i]))
            return 0;
    }

    /* figure out each axis' speed of moving, and sstride */
    for (i = 0; i < ndims; i ++) {
        for (j = 0; j < ndims; onestep[j] = 0, j ++);
        onestep[i] = 1;
        fastrank[i].id = i;
        fastrank[i].priority = ifunc(ndims,dsizes,onestep);    
    }
    qsort(fastrank,ndims,sizeof(VCBpqe),compare);
    
    /* figure out elesize, sstride, blksize */
    elesize  = nattribs * valsize;
    sstride  = ubounds[fastrank[0].id] - lbounds[fastrank[0].id] + 1;
    for (i = 0, blksize = 1; i < ndims; i ++) {
        blksize *= (ubounds[i] - lbounds[i] + 1);
   }

    blkptr   = (char *) boutdata;
    indata   = (char *) bindata;
    memset(blkptr, '\0', blksize * elesize); /* clear every bit to zero */

    for (i = 0; i < ndims; i ++) {
        ubounds[i] = VCB_MINVAL (ubounds[i], dsizes[i]-1);
        lbounds[i] = VCB_MAXVAL (lbounds[i], 0);
    }

    /* grab stride */
    gstride  = ubounds[fastrank[0].id] - lbounds[fastrank[0].id] + 1;
    last     = ifunc(ndims, dsizes, ubounds);

    for (i = 0; i < ndims; clbounds[i] = lbounds[i], i ++) ;
    while ((first = ifunc(ndims, dsizes, clbounds)) < last) {
        memcpy(blkptr, &indata[first*elesize], gstride*elesize);
        blkptr += sstride*elesize;
        blksize += gstride;
        if (incr_indx(clbounds, ndims, lbounds, ubounds, fastrank))
            break; /* when incr_indx == 1, we have an overflow, time to end loop */
    }

    return blksize;
}

int vcbFileGrablk(FILE * bindata, long firstbyte,
                void * boutdata,
                int nattribs, int valsize,      
                int ndims, int * dsizes,      
                int * lbounds, int * ubounds, 
                int (* indexfunc)(int, int *, int *))
/* 
 * grablk_f stands for grab block from file
 * inherently, as well as the API, is concerned, this function is the same as
 * its counterpart grablk, which reads a block from a serialized multi-dimensional
 * dataset in memory. So, please refer to grablk for detailed description of the
 * API.
 *
 * the only difference is that "void * bindata" becomes "FILE * bindata".
 *    bindata is an opened stream containing the multi-dimensional dataset
 *    firstbyte tells where in the dataset is the firstbyte of the volume itself; often
 *         dataset files has quite complex headers (firstbyte is where the volume starts)
 *
 * Note: bindata must be already opened
 *       If the header of the data file is ascii, although the volume data itself is
 *       binary, the safe way to get the value of firstbyte is to move the stream
 *       pointer to the end of the ascii part by using fgets, etc., and then call
 *            firstbyte = ftell(bindata);
 * 
 */
{
    int i, j;
    int onestep[10];                /* assuming at most do ten dimension datasets */
    VCBpqe fastrank[10];             /* how fast does each axis vary, slowest -> 1 */
    int elesize;                    /* size of each data element, in bytes */
    int clbounds[10];

    /* these following locations are measured in number of data elements, like pixel, voxel */
    int first, last;                /* location of 1st and last read */
    int gstride, sstride;           /* stride to grab, step in block */
    int blksize;                    /* size of block to read, consider clipping */

    char * blkptr = NULL;
    char * indata;

    VCB_indexfunc ifunc;

    ifunc = vcbDefaultIndex;
    if (indexfunc != NULL) ifunc = indexfunc;

    if (bindata == NULL) {
        fprintf(stderr,"grab_block_f got given NULL stream bindata to grab from\n");
        return -1;
    }

    if (fseek(bindata, firstbyte, SEEK_SET) < 0) {
        fprintf(stderr,"grab_block_f cannot seek in the given stream to grab from\n");
        return -1;
    }

    if (ndims < 1) {
        fprintf(stderr,"grab_block got %d dimension bindata to grab from, don't know how to handle\n", ndims);
        return -1;
    }

    for (i = 0; i < ndims; i ++) {
        if (dsizes[i] < 0) {
            fprintf(stderr,"grab_block got negative bindata dimension size, dim_%d = %d\n",i,dsizes[i]);
            return -1;
        }

        if (lbounds[i] > ubounds[i]) {
            fprintf(stderr,"grab_block got improper dim_%d bounds (lower, upper) = (%d,%d)\n",i,lbounds[i],ubounds[i]);
            return -1;
        }

        if ((ubounds[i] < 0) || (lbounds[i] > dsizes[i]))
            return 0;
    }

    /* figure out each axis' speed of moving, and sstride */
    for (i = 0; i < ndims; i ++) {
        for (j = 0; j < ndims; onestep[j] = 0, j ++);
        onestep[i] = 1;
        fastrank[i].id = i;
        fastrank[i].priority = ifunc(ndims,dsizes,onestep);    
    }
    qsort(fastrank,ndims,sizeof(VCBpqe),compare);
    
    /* figure out elesize, sstride, blksize */
    elesize  = nattribs * valsize;
    sstride  = ubounds[fastrank[0].id] - lbounds[fastrank[0].id] + 1;
    for (i = 0, blksize = 1; i < ndims; i ++)
        blksize *= (ubounds[i] - lbounds[i] + 1);

    blkptr   = (char *) boutdata;
    indata   = (char *) bindata;
    memset(blkptr, '\0', blksize * elesize); /* clear every bit to zero */

    for (i = 0; i < ndims; i ++) {
        ubounds[i] = VCB_MINVAL (ubounds[i], dsizes[i]-1);
        lbounds[i] = VCB_MAXVAL (lbounds[i], 0);
    }

    /* grab stride */
    gstride  = ubounds[fastrank[0].id] - lbounds[fastrank[0].id] + 1;
    last     = ifunc(ndims, dsizes, ubounds);

    for (i = 0; i < ndims; clbounds[i] = lbounds[i], i ++) ;
    while ((first = ifunc(ndims, dsizes, clbounds)) < last) {
        fseek(bindata, (long)(first*elesize + firstbyte), SEEK_SET);
        fread(blkptr, sizeof(char), gstride*elesize, bindata);
        /* modified from vcbGrablk, which uses the line below instead of the above 2 lines */
        /* memcpy(blkptr, &indata[first*elesize], gstride*elesize); */
        blkptr += sstride*elesize;
        blksize += gstride;
        if (incr_indx(clbounds, ndims, lbounds, ubounds, fastrank))
            break; /* when incr_indx == 1, we have an overflow, time to end loop */
    }

    return blksize;
}

/* ------------------------------------------------------------------------- */

int vcbIndexXYZEtc(int ndims, int *dsizes, int *coords) {
               
   int i;                      /* Counter. */
   int id = coords[0];         /* Initialize for case of 1 dimension. */
   int num_in_prev_dims = 1;   /* Initialize to identity. */
   
   /* Many volumes are indexed in the following manner:
    *    Vox 1: (0 0 0), Vox 2: (1 0 0), Vox 3 (2 0 0)...
    * Given this scheme, this function calculates the id. */

   for (i = 1; i < ndims; i++) {

      /* Make sure coordinate is >= 0 and < dsizes[i]. */
      if (coords[i] < 0 || coords[i] >= dsizes[i]) {
         fprintf(stderr, "vcbIndexXYZEtc: coordinates exceed volume.\n");
         exit(1);
      }

      /* Add the number of voxels required by the previous dimension. */
      num_in_prev_dims *= dsizes[i - 1];
      id += coords[i] * num_in_prev_dims;
   }
   
   /* Give the id back. */
   return id;

}

/* ------------------------------------------------------------------------- */

void vcbIndexXYZEtcToCoords(int ndims, int *dims, int index, int *coords) {

   /* Map an index back to its coordinates. */

   int j;
   int divisor = 1;

   for (j = 0; j < ndims; j++) {
      /* Strip off the previous dimensions' layers and wrap along current
       * layer to find the voxel's position in current dimension. */
      coords[j] = (index / divisor) % dims[j];

      /* Add current dimension's layer of voxels. */
      divisor *= dims[j];
   }

}

/* ------------------------------------------------------------------------- */

int *vcbVoxelsBetweenPoints(int ndims, int *dsizes, float *p1, float *p2,
                            int *nvoxels,
                            int (*index_function) (int, int *, int *)) {

   float *pn;
   int *pn_int;
   int *p1_int;
   int *p2_int;
   float *coeffs;
   int max_coeff;
   int i;
   float num_steps;
   float smallest_num_steps;
   int smallest_num_steps_dim = 0;
   int *ids;
   int done;

   /* We start with no voxels. */
   *nvoxels = 0;

   /* Make room for the coefficients for each dimension. */
   coeffs = (float *) malloc(sizeof(float) * ndims);
   if (coeffs == NULL) {
      fprintf(stderr, "Couldn't allocate memory in voxelsBetweenPoints.\n");
      exit(1);
   }

   /* Make room for the coordinates stepped through along the line and the
    * integral counterparts. */
   pn = (float *) malloc(sizeof(float) * ndims);
   if (pn == NULL) {
      fprintf(stderr, "Couldn't allocate memory in voxelsBetweenPoints.\n");
      exit(1);
   }

   p1_int = (int *) malloc(sizeof(int) * ndims);
   if (p1_int == NULL) {
      fprintf(stderr, "Couldn't allocate memory in voxelsBetweenPoints.\n");
      exit(1);
   }

   p2_int = (int *) malloc(sizeof(int) * ndims);
   if (p2_int == NULL) {
      fprintf(stderr, "Couldn't allocate memory in voxelsBetweenPoints.\n");
      exit(1);
   }

   pn_int = (int *) malloc(sizeof(int) * ndims);
   if (pn_int == NULL) {
      fprintf(stderr, "Couldn't allocate memory in voxelsBetweenPoints.\n");
      exit(1);
   }

   /* Assign the default indexing function, if necessary. */
   if (index_function == NULL) {
      index_function = vcbDefaultIndex;
   }

   /* Determine the coefficients by finding the difference between the start
    * and end value of the two points and initialize the the stepping point as
    * the start point. */
   max_coeff = 0;
   for (i = 0; i < ndims; i++) {
      pn[i] = p1[i];
      pn_int[i] = p1_int[i] = (int) floor(pn[i]);
      p2_int[i] = (int) floor(p2[i]);
      coeffs[i] = (float)ceil((double)(p2[i] - p1[i]));
      max_coeff += (int) fabs(coeffs[i]) + 1;
   }

   /* Make room for the maximum number of cells that the line may possibly
    * pass through.  This isn't asymptotically tight, I don't believe, and
    * someone somewhere has probably proven the actual maximum.  Lines at
    * the correct angle are able to pass through just about all the cells
    * along each dimension, so this is a close approximation. */
   ids = (int *) malloc(sizeof(int) * max_coeff);
   if (ids == NULL) {
      fprintf(stderr, "Couldn't allocate memory in vcbVoxelsOnLine.\n");
      exit(1);
   }

   done = 0;
   while (1) {

      /* I've been indoctrinated to hate break statements, but since this 
       * check needs to be done for both the initial and intermediate values of
       * pn_int and since that check can't be done compactly without resorting
       * to a function call, I've employed a break here.  Ada's exit statement,
       * which allows more arbitrary loop control, should be migrated to C. */

      /* See if any of the coordinates has gone outside the endpoints.  This
       * comparison is why we have integral copies of the endpoints and 
       * intermediate point.  Comparing floats just gets messy and imprecise. */
      for (i = 0; !done && i < ndims; i++) {
         if ((pn_int[i] < p1_int[i] && pn_int[i] < p2_int[i]) ||
             (pn_int[i] > p1_int[i] && pn_int[i] > p2_int[i]) ||
             (pn_int[i] < 0) || (pn_int[i] > dsizes[i] - 1)) {
            done = 1;
         }
      }

      /* If we've gone beyond an endpoint, quit. */
      if (done) {
         break;
      }
      
      /* Otherwise, look up the voxel id for these coordinates and add it
       * to the list. */
      ids[*nvoxels] = index_function(ndims, dsizes, pn_int);
      (*nvoxels)++;

      /* Find where we cross the next gridline, or integral value.  That
       * constitutes a new voxel that we need to add to the list.  How this
       * works is that we check each dimension and see where the next integral
       * crossing is (floor(...)).  We then see what the difference between the
       * current location is (floor() - pn[i]).  Then we divide by the
       * dimension's coefficient to see how many steps it will take to get to
       * the next integral crossing.  The dimension using the fewest steps
       * wins. */
      for (i = 0; i < ndims; i++) {
         if (coeffs[i] < 0) {
            num_steps = fabs((float)((floor((double)(pn[i] - 1.0f)) - pn[i]) /
                                     coeffs[i]));
         } else {
            num_steps = fabs((float)((floor((double)(pn[i] + 1.0f)) - pn[i]) /
                                     coeffs[i]));
           }
         if (i == 0 || num_steps < smallest_num_steps) {
            smallest_num_steps = num_steps;
            smallest_num_steps_dim = i;
         }
      }

      /* Now that we know along which dimension we will next see an integral
       * value, move in all dimensions accordingly to that point. */
      for (i = 0; i < ndims; i++) {
         pn[i] = pn[i] + coeffs[i] * smallest_num_steps;
         pn_int[i] = (int) floor(pn[i]);
      }
   }
   
   /* These are just bookkeeping variables, so we can release this memory
    * back into the wild. */
   free(coeffs);
   free(pn);
   free(pn_int);
   free(p1_int);
   free(p2_int);

   return ids;

}

/* ------------------------------------------------------------------------- */

void * vcbNewVolume(int ndims, int *dsizes, int nattribs, int valsize) {

   void *new_volume;   /* Pointer to new volume. */
   int nvoxels;        /* Number of voxels in volume. */
   int voxel_size;     /* Space required per voxel. */
   int i;              /* Counter. */

   /* Right dimensions? */
   if (ndims < 1) {
      fprintf(stderr, "vcbVoxelsIntoVolume: ndims cannot be < 1.\n");
      exit(1);
   }

   /* Determine the space requirements for a single voxel, and then count
    * up how many total voxels are in the volume, using the dimensions. */
   voxel_size = valsize * nattribs;
   nvoxels = dsizes[0];
   for (i = 1; i < ndims; i++) {
      nvoxels *= dsizes[i];
   }

   /* Now allocate (and clear) space for nvoxels of size voxel_size. */
   new_volume = calloc(nvoxels, voxel_size);
   if (new_volume == NULL) {
      fprintf(stderr, "vcbVoxelsIntoVolume: couldn't allocate memory.\n");
      exit(1);
   }

   /* Give the address back to the user. */
   return new_volume;

}

/* ------------------------------------------------------------------------- */

void vcbVoxelsIntoVolume(int nids, int *ids, void *src, void *dst, int ndims,
                         int *dsizes, int nattribs, int valsize) {

   int nvoxels;     /* Number of voxels in either volume. */
   int voxel_size;  /* Space requirements per voxel. */
   int i;           /* Counter. */
   int offset;      /* Memory displacement for a voxel. */
   int volume_size; /* Total memory consumption for volume. */

   /* Check for legal number of dimensions. */
   if (ndims < 1) {
      fprintf(stderr, "vcbVoxelsIntoVolume: ndims cannot be < 1.\n");
      exit(1);
   }

   /* Determine space requirements per voxel and number of voxels. */
   voxel_size = valsize * nattribs;
   nvoxels = dsizes[0];
   for (i = 1; i < ndims; i++) {
      nvoxels *= dsizes[i];
   }

   /* How much total space does the volume consume?  We need this only to 
    * verify that the ids given fit in the volume specified. */
   volume_size = nvoxels * voxel_size;

   /* Copy the voxel attributes for each id from the source volume to the
    * destination volume. */
   for (i = 0; i < nids; i++) {
      offset = ids[i] * voxel_size;
      if (offset > volume_size) {
         fprintf(stderr, "vcbVoxelsIntoVolume: index exceeds volume size.\n");
         exit(1);
      }
      memcpy(((unsigned char *)dst) + offset, ((unsigned char *)src) + offset, voxel_size);
   }

}

/* ------------------------------------------------------------------------- */

void vcbVolumeFromFunction(int src_ndims, int *src_dsizes, void *src,
                           int dst_ndims, int *dst_dsizes, void *dst,
                           int voxel_size,
                           void (* dst_src_index_function) (int, int *, int *,
                                                            int, int *, int *,
                                                            int *, int *)) {

   int i, j;            /* Counters. */
   int *dst_coords;     /* Coordinates in dst volume. */
   int *src_coords;     /* Coordinates in src volume. */
   int dst_index;       /* Index of voxel at dst_coords in dst volume. */
   int src_index;       /* Index of voxel at src_coords in src volume. */
   int nvoxels = 1;     /* The total number of voxels in dst.  This will 
                         * calculated with a loop, so initialize to 1. */

   /* Lower left-hand coordinates for each cell in the destination volume
    * need to be allocated.  These will be passed to the user's function to
    * get the destination index. */
   dst_coords = (int *) malloc(sizeof(int) * dst_ndims);
   if (dst_coords == NULL) {
      fprintf(stderr, "vcbVolumeFromFunction: couldn't allocate memory.\n");
      exit(1);
   }

   /* Lower left-hand coordinates for each cell in the source volume
    * need to be allocated.  These are simply allocated but not assigned
    * in this function.  The user should calculate the src_coords from the
    * dst_coords and then look up the calculated src_coords index in his
    * passed function. */
   src_coords = (int *) malloc(sizeof(int) * src_ndims);
   if (src_coords == NULL) {
      fprintf(stderr, "vcbVolumeFromFunction: couldn't allocate memory.\n");
      exit(1);
   }

   /* We need to determine how many total voxels there are in the destination
    * volume, and we can go ahead and initialize the first dst_coords to the
    * origin. */
   for (i = 0; i < dst_ndims; i++) {
      nvoxels *= dst_dsizes[i];
      dst_coords[i] = 0;
   }

   /* Now, for each voxel in the destination volume, we must find which
    * src voxel maps to it. */
   for (i = 0; i < nvoxels; i++) {

      /* Given destination ndims, dsizes, coords, and src ndims and dsizes 
       * (src_coords hasn't been assigned; it's there for users), what are
       * the dst_ and src_indexes? */
      dst_src_index_function(dst_ndims, dst_dsizes, dst_coords,
                             src_ndims, src_dsizes, src_coords,
                             &dst_index, &src_index);

      /* Move the indexed src voxel to dst. */
      if (dst_index != -1 && src_index != -1) {
         memcpy(((unsigned char *)dst) + dst_index * voxel_size,
                ((unsigned char *)src) + src_index * voxel_size,
                voxel_size);
      }

      /* Now comes the slightly obscure part of advancing through the volume.
       * Since this volume is of arbitrary dimensions, we can't just use a 
       * series of nested for loops.  Instead, we add 1 to coords[0] and % by
       * the extent along this dimension.  If we come back to 0, then we need
       * to add 1 to coords[1] and % by the extent along that dimension.  If
       * we come back to 0, then we need to add 1 to coords[2] and so on and
       * so forth.  This code behaves like an odometer where each digit is
       * of arbitrary base (dst_dsizes[i]).  As one digit ticks over, it's
       * neighbor a unit higher is incremented, possibly ticking over also.
       * If the digit doesn't tick over, the higher units are left alone. */
      dst_coords[0] = (dst_coords[0] + 1) % dst_dsizes[0];
      for (j = 0; dst_coords[j] == 0 && j < dst_ndims - 1; j++) {
         dst_coords[j + 1] = (dst_coords[j + 1] + 1) % dst_dsizes[j + 1];
      }

   }

}

/* ------------------------------------------------------------------------- */
