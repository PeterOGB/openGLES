/*
Example 1
Copyright (C) 2013  Peter Onion  ( Peter.Onion@btinternet.com )

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

/* Some parts of this source code are based on :

The "hello_pi" examples found in /opt/vc/src/hello_pi on Raspbian

The example code supplied with "OpenGL® ES 2.0 Programming Guid"
See http://opengles-book.com/

The RaspberryPi port of that example code found at 
https://github.com/benosteen/opengles-book-samples/tree/master/Raspi
*/



#include <stdio.h>
#include <stdbool.h>
#include <GLES2/gl2.h>
#include <gtk/gtk.h>

//#include <linux/input.h>
//#include <dirent.h>
//#include <regex.h>

#include "toolkit.h"


// Definition of a wire frame box with an "F" on one face

static  GLfloat linesVertices[] = { 
    -0.5f, -0.5f, 0.5f, 
    0.5f,  -0.5f, 0.5f,

    0.5f,  -0.5f, 0.5f,
    0.5f,  0.5f, 0.5f,

    0.5f,  0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,

    -0.5f,  0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f, 


//24
   -0.5f, -0.5f, -0.5f, 
    0.5f,  -0.5f, -0.5f,

    0.5f,  -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,

    0.5f,  0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, 

// 48
    -0.5f, -0.5f, 0.5f, 
    -0.5f, -0.5f, -0.5f,

    0.5f,  -0.5f, 0.5f,
    0.5f,  -0.5f, -0.5f,

    0.5f,  0.5f, 0.5f,
    0.5f,  0.5f, -0.5f,

    -0.5f,  0.5f, 0.5f,
    -0.5f,  0.5f, -0.5f,

    -0.2f,   -0.4f,    0.5f,
    -0.2f,   0.4f,     0.5f,
    -0.2f,   0.0f,     0.5f,
     0.1f,   0.0f,     0.5f, 
    -0.2f,   0.4f,     0.5f,
     0.2f,   0.4f,     0.5f, 
     


};


POINT Camera = {0.0,0.0,-5.0};

MATRIX_4x4 ModelViewProjectionMatrix;
MATRIX_4x4 ModelViewMatrix;
MATRIX_4x4 PerspectiveMatrix;

// These are used to load values into the shaders
GLint ModelViewProjectionMatrixLocation;
GLint RotateMatrixLocation;
GLint MoveMatrixLocation;


GLint linesPositionLoc; 
GLuint linesProg;

GLint u_ColourLocG;

int frameCount = 0;

// Setup things that don't change frpm frame to frame
gboolean InitScene(void)
{
    // Load the shaders
    linesProg = loadProgram("vertexShader.c","fragmentShader.c");

    
    // Get locations of shader parameters
    linesPositionLoc = glGetAttribLocation(linesProg, "a_position" );
    ModelViewProjectionMatrixLocation = glGetUniformLocation( linesProg, "u_MVPMatrix" );
    MoveMatrixLocation =  glGetUniformLocation( linesProg, "u_moveMatrix" );
    RotateMatrixLocation =  glGetUniformLocation( linesProg, "u_rotateMatrix" );

    u_ColourLocG = glGetUniformLocation(linesProg, "u_ColourG" );
    
    // Calculate the Perspective transformation matrix
    perspectiveView(&PerspectiveMatrix,windowWidth,windowHeight,
    	    30.0f,1.0,10.0);

    // Set the background to black.
    glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
    //glClear( GL_COLOR_BUFFER_BIT );
    return TRUE;
}

// This is called once per frame. It is called before Draw.
// Change global things in here, like moving the camera ......
void Update(void)
{

    static float theta = 0.0;

    frameCount += 1;

    
    identityToMatrix(&ModelViewMatrix);
    // Move the camera away from the origin 
    moveCamera(&ModelViewMatrix,Camera.x,Camera.y,Camera.z);
    // And rotate it around the scene while still looking at the origin
    Rotate(&ModelViewMatrix,theta,0.0,1.0,0.0);
    
    // Form the MVP matrix.
    multiplyMatrix(&ModelViewProjectionMatrix,&ModelViewMatrix,&PerspectiveMatrix);

    // Move the camera a bit furhter round its circular path.
    theta += 0.05f;
}




// Called once per frame.  Use openGL calls etc to draw the scene.
void Draw(void)
{
    static float theta[3] = {0.0,0.0,0.0};
    static int control = 0;
    MATRIX_4x4 RotateMat;
    static int colour = 0;

    // Once every 360 frames change the combination of rotations
    if((frameCount % 360) == 0) control += 1;

    // Setup 

    glClear( GL_COLOR_BUFFER_BIT );
    
    // Select the required shader programme
    glUseProgram(linesProg);

    // Tell the programme to uUse the verticies in array "linesVertices"  
    glVertexAttribPointer((GLuint) linesPositionLoc,3,GL_FLOAT,GL_FALSE,0,linesVertices);
    glEnableVertexAttribArray((GLuint) linesPositionLoc);

    // Load the MVP matrix
    glUniformMatrix4fv( ModelViewProjectionMatrixLocation, 1, GL_FALSE, 
			(GLfloat*) &ModelViewProjectionMatrix.elements[0][0] );

    colour += 1;
    colour &= 1;
    switch(colour)
    {
	case 0:
	    glUniform4f(u_ColourLocG,1.0,0.0,0.0,1.0);
	    break;
	case 1:
	    glUniform4f(u_ColourLocG,0.0,1.0,0.0,1.0);
	    break;
	case 2:
	    glUniform4f(u_ColourLocG,0.0,0.0,1.0,1.0);
	    break;
	case 3:
	    glUniform4f(u_ColourLocG,1.0,1.0,1.0,1.0);
	    break;	    
    }

	    

    // DRAW RIGHT CUBE
    // Clear the rotation matrix
    identityToMatrix(&RotateMat);
    // Rotate about Y axis
    Rotate(&RotateMat,theta[0],0.0,1.0,0.0);
    // load RotateMat into the RotateMatrix in the shaders 
    glUniformMatrix4fv( RotateMatrixLocation, 1, GL_FALSE, 
			(GLfloat*) &RotateMat.elements[0][0] );
    // Move the object to the right by 1.207 (0.500+0.707) 
    glUniform4f(MoveMatrixLocation,1.207f,0.0,0.0,0.0);
    // Draw a cube
    glDrawArrays ( GL_LINES, 0, 30 );


    // DRAW MIDDLE CUBE
    // Clear the rotation matrix
    identityToMatrix(&RotateMat);
    // Rotate about X axis
    Rotate(&RotateMat,theta[1],1.0,0.0,0.0);
    // load RotateMat into the RotateMatrix in the shaders 
    glUniformMatrix4fv( RotateMatrixLocation, 1, GL_FALSE, 
			(GLfloat*) &RotateMat.elements[0][0] );

    // Load (0,0,0) into the MoveMatrix in the shaders
    glUniform4f(MoveMatrixLocation,0.0,0.0,0.0,0.0);
    // Draw a cube
    glDrawArrays ( GL_LINES, 0, 30 );


    // DRAW LEFT CUBE
    // Clear the rotation matrix
    identityToMatrix(&RotateMat);
    // Rotate about Z axis
    Rotate(&RotateMat,theta[2],0.0,0.0,1.0);
    // load RotateMat into the RotateMatrix in the shaders 
    glUniformMatrix4fv( RotateMatrixLocation, 1, GL_FALSE, 
			(GLfloat*) &RotateMat.elements[0][0] );
    // Move the object to the left 
    glUniform4f(MoveMatrixLocation,-1.207f,0.0,0.0,0.0);
    // Draw a cube
    glDrawArrays ( GL_LINES, 0, 30 );


    // Increase the cubes' rotations depending on the value of variable "control". 
    if(control & 1)  theta[0] += 0.25f;
    if(control & 2)  theta[1] += 0.25f;
    if(control & 4)  theta[2] += 0.25f;

}

