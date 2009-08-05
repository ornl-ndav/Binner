/***************************************************************************
 *   Copyright (C) 2004 by Andrew Gaston                                   *
 *   gaston@cs.utk.edu                                                     *
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


#include <math.h>

#include <string.h>

#include "ArcBall.h"


ArcBall::ArcBall()
{

   m_matrix[0] = m_matrix[5] = m_matrix[10] = m_matrix[15] = 1.0f;
   m_matrix[1] = m_matrix[2] = m_matrix[3] = m_matrix[4] = 0.0f;
   m_matrix[6] = m_matrix[7] = m_matrix[8] = m_matrix[9] = 0.0f;
   m_matrix[11] = m_matrix[12] = m_matrix[13] = m_matrix[14] = 0.0f;

   m_axis[0] = 1.0f;
   m_axis[1] = 0.0f;
   m_axis[2] = 0.0f;
   m_angle = 0.0f;

   m_width = 0;
   m_height = 0;

   m_track = false;
   m_cUpdate = false;

}


ArcBall::~ArcBall()
{
}


void ArcBall::SetWidth(int width)
{

   m_width = width;

}


void ArcBall::SetHeight(int height)
{

   m_height = height;

}


int ArcBall::GetWidth()
{

   return m_width;

}


int ArcBall::GetHeight()
{

   return m_height;

}


void ArcBall::Update()
{

   float tmat[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f};

   float res[16];
   float fac;
   float vec[3];

   if ( (!m_cUpdate) && ( (!m_track) || (!m_trackDirty)))
   {

      //no need to update
      return;

   }

   m_trackDirty = false;

   fac = m_axis[0]*m_axis[0] + m_axis[1]*m_axis[1] + m_axis[2]*m_axis[2];

   if (fac>0.0f)
   {

      fac = 1.0f / sqrtf(fac);

   }

   vec[0] = fac * m_axis[0];
   vec[1] = fac * m_axis[1];
   vec[2] = fac * m_axis[2];

   float s,v;
   float a2, b2, c2, a, b, c;

   //compute axis/angle rotation matrix
   s = cosf(m_angle/2.0f);
   v = sinf(m_angle/2.0f);
   a = v*vec[0];
   b = v*vec[1];
   c = v*vec[2];
   a2 = a*a;
   b2 = b*b;
   c2 = c*c;

   tmat[0] = 1.0f - 2.0f*b2 - 2.0f*c2;
   tmat[1] = 2.0f*a*b + 2.0f*s*c;
   tmat[2] = 2.0f*a*c - 2.0f*s*b;
   tmat[4] = 2.0f*a*b - 2.0f*s*c;
   tmat[5] = 1.0f - 2.0f*a2 - 2.0f*c2;
   tmat[6] = 2.0f*b*c + 2.0f*s*a;
   tmat[8] = 2.0f*a*c + 2.0f*s*b;
   tmat[9] = 2.0f*b*c - 2.0f*s*a;
   tmat[10] = 1.0f - 2.0f*a2 - 2.0f*b2;

   //perform matrix multiply
   for (int ii=0; ii<4; ii++)
   {
      for (int gg=0; gg<4; gg++)
      {

         res[ii*4+gg] = m_matrix[ii*4 + 0] * tmat[gg] + m_matrix[ii*4 + 1] * tmat[gg + 4] +
                        m_matrix[ii*4 + 2] * tmat[gg + 8] + m_matrix[ii*4 + 3] * tmat[gg + 12];

      }

   }

   memcpy( m_matrix, res, sizeof(float)*16);

}


float* ArcBall::GetMatrix()
{

   return m_matrix;

}


void ArcBall::StartMotion( int x, int y, int time)
{

   m_track = true;
   m_cUpdate = false;

   m_lastTime = time;

   Project(x, y, m_pos);

}


void ArcBall::StopMotion( int x, int y, int time)
{

   m_track = false;

   if (time == m_lastTime)
   {

      m_cUpdate = true;

   }

}


void ArcBall::TrackMotion( int x, int y, int time)
{

   float curPos[3], dx, dy, dz;

   if (!m_track)
   {

      return;

   }

   m_trackDirty = true;

   Project(x, y, curPos);

   dx = curPos[0] - m_pos[0];
   dy = curPos[1] - m_pos[1];
   dz = curPos[2] - m_pos[2];

   m_angle = 3.1415927f/2.0f * sqrtf(dx*dx + dy*dy + dz*dz);

   m_axis[0] = m_pos[1]*curPos[2] - m_pos[2]*curPos[1];
   m_axis[1] = m_pos[2]*curPos[0] - m_pos[0]*curPos[2];
   m_axis[2] = m_pos[0]*curPos[1] - m_pos[1]*curPos[0];

   m_lastTime = time;

   m_pos[0] = curPos[0];
   m_pos[1] = curPos[1];
   m_pos[2] = curPos[2];

}


void ArcBall::Project(int x, int y, float v[3])
{

   float d, a;
   
   // project x,y onto a hemi-sphere centered within width, height
   v[0] = (2.0f*x - m_width) / m_width;
   v[1] = (m_height - 2.0f*y) / m_height;

   d = sqrtf(v[0]*v[0] + v[1]*v[1]);

   v[2] = cosf((3.1415927f/2.0f) * ((d < 1.0f) ? d : 1.0f));

   a = 1.0f / sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

   v[0] *= a;
   v[1] *= a;
   v[2] *= a;

}


void ArcBall::GetInvMatrix(float *m_inv)
{

  float *m;

  m = GetMatrix();

  for (int j=0; j<4; j++) {
    m_inv[j*4+0] = m[0*4+j];
    m_inv[j*4+1] = m[1*4+j];
    m_inv[j*4+2] = m[2*4+j];
    m_inv[j*4+3] = m[3*4+j];

  }

}


void ArcBall::XformVector(float &xo, float &yo, float &zo, float xi, float yi, float zi)
{

	float dst[4], *m;

	m = GetMatrix();

	for (int i=0; i<3; i++) {

		dst[i] = m[i]*xi + m[i+4]*yi + m[i+8]*zi;

	}

	xo = dst[0];
	yo = dst[1];
	zo = dst[2];

}


void ArcBall::InvXformVector(float &xo, float &yo, float &zo, float xi, float yi, float zi)
{

	float dst[4], m_inv[16];

	GetInvMatrix(m_inv);

  for (int i=0; i<3; i++) {

    dst[i] = m_inv[i]*xi + m_inv[i+4]*yi + m_inv[i+8]*zi;

  }

	xo = dst[0];
	yo = dst[1];
	zo = dst[2];

}
