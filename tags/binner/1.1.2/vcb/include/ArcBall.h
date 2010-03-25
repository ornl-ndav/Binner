/****************************************************************************
 *   Copyright (C) 2004  by Andrew Gaston									*
 *	 gaston@cs.utk.edu														*
 *																			*
 *   This library is free software; you can redistribute it and/or			*
 *   modify it under the terms of the GNU Lesser General Public				*
 *   License as published by the Free Software Foundation; either			*
 *   version 2.1 of the License, or (at your option) any later version.		*
 *																			*
 *   This library is distributed in the hope that it will be useful,		*
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of			*
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU		*
 *   Lesser General Public License for more details.						*
 *																			*
 *   You should have received a copy of the GNU Lesser General Public		*
 *   License along with this library; if not, write to the					*
 *   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,			*
 *   Boston, MA  02110-1301  USA											*
 ****************************************************************************/

#ifndef ARCBALL_H

#define ARCBALL_H

/** Directions for Arcball
 *  <h2>Description:</h2>
 *  ArcBall is an intuitive way of viewing a 3-D object (or scene) such that when
 *  you move your mouse, the object (or scene) rotates accordingly. You can think
 *  of it as if instead of your mouse moving on your two dimentional monitor screen
 *  it was instead moving along the surface of a three dimentional sphere. It works
 *  by mapping mouse coordinates onto ArcBall's sphere coordinates, as if it were
 *  directly in front of you.
 *
 * <h2>See the complete documentation for Arcball <a href=arcball/index.html>here</a></h2>
 *	
 *	<h2>Example usage of ArcBall:</h2><pre>
 * #include "ArcBall.h"
 *
 * ArcBall Spaceball;
 * 
 * int main(int argc, char **argv) {
 *     //your code here
 *     init();
 *     //your code here
 *     glutDisplayFunc(display);
 *     glutReshapeFunc(reshape);
 *     glutIdleFunc(display);
 *     glutMouseFunc(mouse_handler);
 *     glutMotionFunc(trackMotion);
 *     //your code here
 * }
 *
 * void reshape(void) {
 *     //your code here
 *     Spaceball.SetWidth(width);
 *     Spaceball.SetHeight(height);
 *     //your code here
 * }
 *
 * void display(void) {
 *     //your code here
 *     Spaceball.Update();
 *     //your code here
 *     glMultMatrixf(Spaceball.GetMatrix());
 *     //your code here
 * }
 * void mouse_handler(int button, int bstate, int x, int y) {
 *     //your code here
 *     if (button == GLUT_LEFT_BUTTON) {
 *         if (bstate == GLUT_DOWN) {
 *             Spaceball.StartMotion( x, y, glutGet(GLUT_ELAPSED_TIME));
 *         }
 *         else if (bstate == GLUT_UP) {
 *             Spaceball.StopMotion( x, y, glutGet(GLUT_ELAPSED_TIME));
 *         }
 *     }
 * }
 *
 * void trackMotion(int x, int y) {
 *    //your code here
 *    if (Spaceball.Track())
 *        Spaceball.TrackMotion( x, y, glutGet(GLUT_ELAPSED_TIME));
 *    //your code here
 * }
 *</pre>
 **/

/**
 * \brief ArcBall holds all of the data and functions that ArcBall uses to rotate the scene.
 * 
 * You must declare an ArcBall object in your code to use the associated functionality.
 */
class ArcBall

{

   public:

      /**
       * \brief ArcBall is the constructor for the ArcBall class.  It initializes all of the ArcBall's variables to their initial values.
       * 
       * The constructor initializes ArcBall's rotation matrix to the identity matrix, initializes ArcBall's rotation axis and sets the
       * rotation angle to 0, and initializes ArcBall's bounding box's width to 0 and height to 0.
       */
      ArcBall();

      ~ArcBall();    /**< \brief Default Destructor */


      /**
       * \brief SetWidth sets the width of the ArcBall's bounding box to "width"
       *
       * You probably want to pass SetWidth the width of the current viewport so that no matter where you drag
       * your mouse in the window the ArcBall will continue to rotate.
       */
      void SetWidth( int width);
      
      /**
       * \brief SetHeight sets the height of the ArcBall's bounding box to "height"
       * 
       * You probably want to pass SetHeight the height of the current viewport so that no matter where you drag
       * your mouse in the window the ArcBall will continue to rotate.
       */
      void SetHeight( int height);



      int GetWidth();    /**< \brief GetWidth returns the current value of the width of the ArcBall's bounding box */

      int GetHeight();    /**< \brief GetHeight returns the current value of the height of the ArcBall's bounding box */


      /**
       * \brief Update updates all the values in the ArcBall class to reflect the current position of the mouse cursor
       *
       * You will most likely want to put this function in a display callback and a idle callback so that the rotation
       * of your scene will be correctly updated with the real world position of the mouse cursor.
       */
      void Update();    


      /**
       * \brief GetMatrix returns the value of the ArcBall's current rotation matrix
       *
       * This is the function that returns the value that you want to multiply your Modelview matrix by.  If you multiply
       * your Modelview matrix by this value before you preform viewport transformations then it will be your whole scene
       * that rotates with your mouse.  If, however, you mulitply your Modelview matrix by this value directly before you
       * draw the object(s) in your scene then it will be the object(s) that rotate.
       */
      float* GetMatrix();

      
      /**
       * \brief GetInvMatrix places the inverse of the ArcBall's current rotation matrix into "m_inv"
       *
       * The inverse of the ArcBall's current rotaton matrix is used in internal calculations.  As such, you won't need to
       * call this function unless you want to see a bunch of meaningless numbers.
       */
      void GetInvMatrix(float *m_inv);

      
      /**
       * \brief StartMotion begins rotation based upon the "x" and "y" values passed to it and records the "time" passed to it.
       * 
       * You will need to place this function in a mouse function callback in order to have your mouse correctly rotate the scene.
       * Since this function starts rotation and does not stop it you will want to invoke this function only while a mouse button
       * is currently pressed.  Also, you will need to pass the elapsed time to the "time" arguement.
       */
      void StartMotion( int x, int y, int time);

      
      /**
       * \brief StopMotion stops rotation based upon the "x" and "y" values passed to it and compares the value of StartMotion's
       * "time" arguement with the "time" passed to it.
       * 
       * You will need to place this function in a mouse function callback in order to have your mouse stop rotating the scene. Since
       * this function stops rotation you will want to invoke this function while a mouse button is currently unpressed.  If the
       * difference between "time" and StartMotion's "time" arguement is less than one millisecond the scene will keep rotating based
       * on the value of the last millisecond of rotation.
       */
      void StopMotion( int x, int y, int time);


      /**
       * \brief TrackMotion is the function that actually tracks the mouse cursor and rotates the scene accordingly.
       * 
       * You will need to place this function in a motion function callback in order to have your scene continously rotate.  TrackMotion
       * uses the "x" and "y" passed to it for rotation.  It also records the time that it was called with the "time" passed to it.
       */
      void TrackMotion( int x, int y, int time);


      /**
       * \brief Track returns a TRUE if mouse tracking has been enabled, FALSE if it has not.
       * 
       * You will need to include this function in a motion function callback in order to have your object's (or scene's) rotation track
       * your mouse's movements.  If Track is true then you will need to call TrackMotion to keep rotating your scene, if Track is false
       * then you do not need to continue to rotate your object (or scene).
       */
      bool Track() {return m_track;};

      
      /**
       * \brief ContinuousUpdate returns a value of TRUE or FALSE which tells you if your scene is being continuously updated or not.
       */
      bool ContinuousUpdate() {return m_cUpdate;};

      /**
       * \brief XformVector is an internal function used in quaternion calculations.
       */
      void XformVector(float &xo, float &yo, float &zo, float xi, float yi, float zi);

      
      /**
       * \brief InvXformVector is an internal function used in quaternion calculations.
       */
      void InvXformVector(float &xo, float &yo, float &zo, float xi, float yi, float zi);


      
   private:


      /**
       * \brief Project is the magic of the ArcBall.  It projects "x" and "y" onto a hemi-sphere centered at m_width, m_height.
       */
      void Project(int x, int y, float v[3]);


      
      float m_matrix[16];    /**< \brief m_matrix is the current rotation matrix being used by the ArcBall. */



      float m_axis[3];    /**< \brief m_axis is the current axis of rotation being used by the ArcBall. */

      float m_angle;    /**< \brief m_angle is the current angle of rotation being used by ArcBall. */



      int m_width;    /**< \brief m_width is the current width of the ArcBall's bounding box. */

      int m_height;    /**< \brief m_height is the current height of the ArcBall's bounding box. */



      int m_lastTime;    /**< \brief m_lastTime is the time that StartMotion or TrackMotion was last called. */



      float m_pos[3];    /**< \brief m_pos is used in internal matrix computations. */



      bool m_track;    /**< \brief m_track is true when mouse tracking is enabled, false when it is disabled */

      bool m_cUpdate;    /**< \brief m_cUpdate is true when coninuous updating of the scene is enabled */

      bool m_trackDirty;    /**< \brief m_trackDirty is true sometimes and false others :) */



};



#endif //ARCBALL_H
