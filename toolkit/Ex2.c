/*
Example 2
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

#include "toolkit.h"


// Definition of an "F" 

static GLfloat effVertices[] = {
    -0.15f,  -0.45f,    0.5f,
    -0.25f,  -0.45f,    0.5f,
    -0.25f,  0.0f,    0.5f,
    -0.25f,  0.45f,    0.5f,
    0.25f,   0.45f,    0.5f,
    0.25f,   0.35f,    0.5f,
    -0.15f,  0.35f,    0.5f,
    -0.15f,  0.05f,    0.5f,
    0.15f,   0.05f,    0.5f,
    0.15f,   -0.05f,   0.5f,
    -0.15f,  -0.05f,   0.5f,
};

// Some of these triangle have the wrong orientation
GLubyte effIndicesWrong[] =
{
    0, 1, 10,
    1, 2, 10,
    2, 7, 10,
    2, 6, 7,
    2, 6, 3,
    6, 3, 4,
    6, 5, 4,
    7, 8, 10,
    10,9, 8
};
    
// All of these triangles have the correct orientation
GLubyte effIndicesCorrect[] =
{
    0,10, 1,
    1,10, 2,
    2,10, 7,
    2, 7, 6,
    2, 6, 3,
    6, 4, 3,
    6, 5, 4,
    8, 7, 10,
    10,9, 8
};    


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


    // Note the lines for drawing the "F" have been moves slightly backwards to Z=0.49.
    // This is so that the red "F" (at Z=0.5)  is drawn infront of the line version.
    -0.2f,   -0.4f,    0.49f,
    -0.2f,   0.4f,     0.49f,
    -0.2f,   0.0f,     0.49f,
     0.1f,   0.0f,     0.49f, 
    -0.2f,   0.4f,     0.49f,
     0.2f,   0.4f,     0.49f, 
     


};


POINT Camera = {0.0,0.0,-5.0};

MATRIX_4x4 ModelViewProjectionMatrix;
MATRIX_4x4 ModelViewMatrix;
MATRIX_4x4 PerspectiveMatrix;

// These are used to load values into the shaders
GLint ModelViewProjectionMatrixLineLocation;
GLint RotateMatrixLineLocation;
GLint MoveMatrixLineLocation;

GLint ModelViewProjectionMatrixTriangleLocation;
GLint RotateMatrixTriangleLocation;
GLint MoveMatrixTriangleLocation;
GLint ColourTriangleLocation;


GLuint linesPositionLoc; 
GLuint linesProg;

GLuint trianglesPositionLoc;
GLuint trianglesProg;

int frameCount = 0;

// Setup things that don't change frpm frame to frame
gboolean InitScene(void)
{
    // Load the lines shaders
    linesProg = loadProgram("vertexLineShader.c","fragmentLineShader.c");

    
    // Get locations of shader parameters
    linesPositionLoc = glGetAttribLocation(linesProg, "a_position" );
    ModelViewProjectionMatrixLineLocation = glGetUniformLocation( linesProg, "u_MVPMatrix" );
    MoveMatrixLineLocation = glGetUniformLocation( linesProg, "u_moveMatrix" );
    RotateMatrixLineLocation = glGetUniformLocation( linesProg, "u_rotateMatrix" );


    

   // Load the triangle shaders
    trianglesProg = loadProgram("vertexTriangleShader.c","fragmentTriangleShader.c");
    
    // Get locations of shader parameters
    trianglesPositionLoc = glGetAttribLocation(trianglesProg, "a_position" );
    ModelViewProjectionMatrixTriangleLocation = glGetUniformLocation( trianglesProg, "u_MVPMatrix" );
    MoveMatrixTriangleLocation = glGetUniformLocation( trianglesProg, "u_moveMatrix" );
    RotateMatrixTriangleLocation = glGetUniformLocation( trianglesProg, "u_rotateMatrix" );
    ColourTriangleLocation = glGetUniformLocation( trianglesProg, "u_triC1" );


    // Calculate the Perspective transformation matrix
    perspectiveView(&PerspectiveMatrix,windowWidth,windowHeight,
    	    30.0f,1.0,10.0);

    // Set the background to black.
    glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );

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
    theta += 0.05;
}




// Called once per frame.  Use openGL calls etc to draw the scene.
void Draw(void)
{
    static float theta[3] = {0.0,0.0,0.0};
    static int control = 0;
    MATRIX_4x4 RotateMat;
    static GLfloat Col[4] = {0.5f,0.0f,0.0f,1.0f};
    
    // Once every 360 frames change the combination of rotations
    if((frameCount % 360) == 0) control += 1;

    // Setup various options 
    glViewport(0,0,windowWidth,windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
    glEnable(GL_DEPTH_TEST);
    glDepthRangef(-5.0f,5.0f); 
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);


    
    // Select the line drawing shader programme
    glUseProgram(linesProg);

    // Tell the programme to uUse the verticies in array "linesVertices"  
    glVertexAttribPointer(linesPositionLoc,3,GL_FLOAT,GL_FALSE,0,linesVertices);
    glEnableVertexAttribArray(linesPositionLoc);

    // Load the MVP matrix
    glUniformMatrix4fv( ModelViewProjectionMatrixLineLocation, 1, GL_FALSE, 
			(GLfloat*) &ModelViewProjectionMatrix.elements[0][0] );

    
 

    // DRAW RIGHT CUBE
    // Clear the rotation matrix
    identityToMatrix(&RotateMat);
    // Rotate about Y axis
    Rotate(&RotateMat,theta[0],0.0,1.0,0.0);
    // load RotateMat into the RotateMatrix in the shaders 
    glUniformMatrix4fv( RotateMatrixLineLocation, 1, GL_FALSE, 
			(GLfloat*) &RotateMat.elements[0][0] );
    // Move the object to the right by 1.207 (0.500+0.707) 
    glUniform4f(MoveMatrixLineLocation,1.207,0.0,0.0,0.0);
    // Draw a cube
    glDrawArrays ( GL_LINES, 0, 30 );

   


    // DRAW MIDDLE CUBE
    // Clear the rotation matrix
    identityToMatrix(&RotateMat);
    // Rotate about X axis
    Rotate(&RotateMat,theta[1],1.0,0.0,0.0);
    // load RotateMat into the RotateMatrix in the shaders 
    glUniformMatrix4fv( RotateMatrixLineLocation, 1, GL_FALSE, 
			(GLfloat*) &RotateMat.elements[0][0] );

    // Load (0,0,0) into the MoveMatrix in the shaders
    glUniform4f(MoveMatrixLineLocation,0.0,0.0,0.0,0.0);
    // Draw a cube
    glDrawArrays ( GL_LINES, 0, 30 );


    // DRAW LEFT CUBE
    // Clear the rotation matrix
    identityToMatrix(&RotateMat);
    // Rotate about Y axis
    Rotate(&RotateMat,theta[2],0.0,1.0,0.0);
    // load RotateMat into the RotateMatrix in the shaders 
    glUniformMatrix4fv( RotateMatrixLineLocation, 1, GL_FALSE, 
			(GLfloat*) &RotateMat.elements[0][0] );
    // Move the object to the left 
    glUniform4f(MoveMatrixLineLocation,-1.207,0.0,0.0,0.0);
    // Draw a cube
    glDrawArrays ( GL_LINES, 0, 30 );



    // Change to the triangle shader 

    // Select the triangle drawing shader programme
    glUseProgram(trianglesProg);
 
    // Tell the programme to uUse the verticies in array "linesVertices"  
    glVertexAttribPointer(trianglesPositionLoc,3,GL_FLOAT,GL_FALSE,0,effVertices);
    glEnableVertexAttribArray(trianglesPositionLoc);

    // Load the MVP matrix
    glUniformMatrix4fv( ModelViewProjectionMatrixTriangleLocation, 1, GL_FALSE, 
			(GLfloat*) &ModelViewProjectionMatrix.elements[0][0] );


    // Set up back face (clockwise) culling
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    // But don't enable it yet
    glDisable(GL_CULL_FACE);

    // DRAW RIGHT CUBE F  (Front and Back facing triangles are shown)

    // Clear the rotation matrix
    identityToMatrix(&RotateMat);
    // Rotate about Y axis
    Rotate(&RotateMat,theta[0],0.0,1.0,0.0);
    // load RotateMat into the RotateMatrix in the shaders 
    glUniformMatrix4fv( RotateMatrixTriangleLocation, 1, GL_FALSE, 
			(GLfloat*) &RotateMat.elements[0][0] );
    // Move the object to the right by 1.207 (0.500+0.707) 
    glUniform4f(MoveMatrixTriangleLocation,1.207,0.0,0.0,0.0);

    
    //glUniform4fv(ColourTriangleLocation,1,(GLfloat *)&Col[0]);
    glUniform4f(ColourTriangleLocation,1.0,0.0,0.0,1.0);
    glDrawElements ( GL_TRIANGLES, 27, GL_UNSIGNED_BYTE, effIndicesWrong );


    // Now enable back face culling
    glEnable(GL_CULL_FACE);

    // DRAW MIDDLE CUBE F (Different subset of triangles are shown when looking
    // from the front and the back

    // Clear the rotation matrix
    identityToMatrix(&RotateMat);
    // Rotate about Y axis
    Rotate(&RotateMat,theta[1],1.0,0.0,0.0);
    // load RotateMat into the RotateMatrix in the shaders 
    glUniformMatrix4fv( RotateMatrixTriangleLocation, 1, GL_FALSE, 
			(GLfloat*) &RotateMat.elements[0][0] );
    // Move the object to the right by 0) 
    glUniform4f(MoveMatrixTriangleLocation,0.0,0.0,0.0,0.0);
    //glUniform4fv(ColourTriangleLocation,1,(GLfloat *)&Col[0]);
    glUniform4f(ColourTriangleLocation,0.0,1.0,0.0,1.0);

    glDrawElements ( GL_TRIANGLES, 27, GL_UNSIGNED_BYTE, effIndicesWrong );

    // DRAW LEFT CUBE F (All triangles are correct orientation so the Red F
    // is only drawn when viewing from the front.

    // Clear the rotation matrix
    identityToMatrix(&RotateMat);
    // Rotate about Y axis
    Rotate(&RotateMat,theta[2],0.0,1.0,0.0);
    // load RotateMat into the RotateMatrix in the shaders 
    glUniformMatrix4fv( RotateMatrixTriangleLocation, 1, GL_FALSE, 
			(GLfloat*) &RotateMat.elements[0][0] );
    // Move the object to the left 
    glUniform4f(MoveMatrixTriangleLocation,-1.207,0.0,0.0,0.0);
    //glUniform4fv(ColourTriangleLocation,1,(GLfloat *)&Col[0]);
    glUniform4f(ColourTriangleLocation,0.0,0.0,1.0,1.0);
    // Use the correctly oriented triangle set.              
    glDrawElements ( GL_TRIANGLES, 27, GL_UNSIGNED_BYTE, effIndicesCorrect );


    // Increase the cubes' rotations depending on the value of variable "control". 
    if(control & 1)  theta[0] += 0.25;
    if(control & 2)  theta[1] += 0.25;
    if(control & 4)  theta[2] += 0.25;

}

