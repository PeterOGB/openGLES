/*
Example 5
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
#include <math.h>
#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>
#include <png.h>
#include <GLES2/gl2.h>
#include <gtk/gtk.h>
#include "toolkit.h"



// Definition of cube verticies
// Each vertex is defined three times as it belongs to three faces
// Each face is made from two triangles. 

static  GLfloat Vertices[] = { 
    -0.5f, -0.5f, 0.5f, 
    0.5f,  -0.5f, 0.5f,
    0.5f,  0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,

    0.5f,  -0.5f,  0.5f,
    0.5f,  -0.5f,  -0.5f,
    0.5f,  0.5f,   -0.5f,
    0.5f,  0.5f,   0.5f,

    0.5f,  -0.5f,  -0.5f,
    -0.5f, -0.5f,  -0.5f,
    -0.5f, 0.5f,   -0.5f,
    0.5f,  0.5f,   -0.5f,

    -0.5f, -0.5f,  -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, 0.5f,   0.5f,
    -0.5f, 0.5f,   -0.5f,

    -0.5f, 0.5f,   0.5f, 
    0.5f,  0.5f,   0.5f,
    0.5f,  0.5f,   -0.5f,
    -0.5f, 0.5f,   -0.5f,

    0.5f,  -0.5f,  -0.5f,
    -0.5f, -0.5f,  -0.5f,
    -0.5f, -0.5f,  0.5f, 
    0.5f,  -0.5f,  0.5f,
};


// Triangle definitions in terms of vertex indices
GLubyte Indices[] = {
    0, 1, 2,
    0, 2, 3,
    4, 5, 6,
    4, 6, 7,
    8, 9, 10,
    8, 10,11,
    12,13,14,
    12,14,15,
    16,17,18,
    16,18,19,
    20,22,21,
    20,23,22,


};

// Locations of the coloured circles in the texture
// These match the order of the vertices 

GLfloat Texture[48] = {
    0.0,     1.0,
    0.125,   1.0,
    0.125,   0.0,
    0.0,     0.0,

    0.125,     1.0,
    0.25,   1.0,
    0.25,   0.0,
    0.125,     0.0,

    0.25,     1.0,
    0.375,   1.0,
    0.375,   0.0,
    0.25,     0.0,

    0.375,     1.0,
    0.5,   1.0,
    0.5,   0.0,
    0.375,     0.0,

    0.5,     1.0,
    0.625,   1.0,
    0.625,   0.0,
    0.5,     0.0,

    0.625,     1.0,
    0.75,   1.0,
    0.75,   0.0,
    0.625,     0.0,



};


GLfloat faceMatch[] = {
    // RED FACES
    +1.0,0.0,0.0,
    0.0,+1.0,0.0,
    0.0,0.0,+1.0,

    0.0,-1.0,0.0,
    +1.0,0.0,0.0,
    0.0,0.0,+1.0,

    0.0,+1.0,0.0,
    -1.0,0.0,0.0,
    0.0,0.0,+1.0,

    -1.0,0.0,0.0,
    0.0,-1.0,0.0,
    0.0,0.0,+1.0,

    // WHITE FACES
    0.0,0.0,-1.0,
    0.0,+1.0,0.0,
    +1.0,0.0,0.0,

    0.0,0.0,-1.0,
    +1.0,0.0,0.0,
    0.0,-1.0,0.0,

    0.0,0.0,-1.0,
    -1.0,0.0,0.0,
    0.0,+1.0,0.0,

    0.0,0.0,-1.0,
    0.0,-1.0,0.0,
    -1.0,0.0,0.0,

    // BLUE FACES
    -1.0,0.0,0.0,
    0.0,+1.0,0.0,
    0.0,0.0,-1.0,

    0.0,+1.0,0.0,
    +1.0,0.0,0.0,
    0.0,0.0,-1.0,

    0.0,-1.0,0.0,
    -1.0,0.0,0.0,
    0.0,0.0,-1.0,

    +1.0,0.0,0.0,
    0.0,-1.0,0.0,
    0.0,0.0,-1.0,

    // GREEN FACES

    0.0,0.0,+1.0,
    0.0,+1.0,0.0,
    -1.0,0.0,0.0,

    0.0,0.0,+1.0,
    +1.0,0.0,0.0,
    0.0,+1.0,0.0,

    0.0,0.0,+1.0,
    -1.0,0.0,0.0,
    0.0,-1.0,0.0,

    0.0,0.0,+1.0,
    0.0,-1.0,0.0,
    +1.0,0.0,0.0,

    // YELLOW FACES
    +1.0,0.0,0.0,
    0.0,0.0,+1.0,
    0.0,-1.0,0.0,

    0.0,-1.0,0.0,
    0.0,0.0,+1.0,
    -1.0,0.0,0.0,

    0.0,+1.0,0.0,
    0.0,0.0,+1.0,
    +1.0,0.0,0.0,

    -1.0,0.0,0.0,
    0.0,0.0,+1.0,
    0.0,+1.0,0.0,

    // ORANGE FACES
    +1.0,0.0,0.0,
    0.0,0.0,-1.0,
    0.0,+1.0,0.0,

    0.0,-1.0,0.0,
    0.0,0.0,-1.0,
    +1.0,0.0,0.0,

    0.0,+1.0,0.0,
    0.0,0.0,-1.0,
    -1.0,0.0,0.0,

    -1.0,0.0,0.0,
    0.0,0.0,-1.0,
    0.0,-1.0,0.0,


};


POINT Camera1 = {-0.0,-0.0,-10.0};
MATRIX_4x4 RotateCube;

MATRIX_4x4 ModelViewProjectionMatrix;

MATRIX_4x4 ModelViewMatrix;
MATRIX_4x4 PerspectiveMatrix;

// These are used to load values into the shaders

GLint ModelViewProjectionMatrixLocation;
GLint RotateFaceMatrixLocation;
GLint RotateBlockMatrixLocation;
GLint MoveMatrixLocation;
GLint ColourTriangleTextureLocation;
GLint TextureTriangleTextureLocation;

GLuint trianglesTextureProg;
GLuint trianglesTexturePositionLoc;
GLint trianglesTextureTexture;
GLint TextureTriangleTexCoordLoc;
GLint RotateMatrixLocation;


int mouseMoveLR = 4;
int mouseMoveUD = 6;
gboolean mousePressLeft = FALSE;
gboolean mousePressRight = FALSE;


struct block
{
  int orientation;
};

// Arrays to store the orientation of each block in the cube.
struct block cube[3][3][3];
struct block cube2[3][3][3];

enum orientations {RED_U=0,RED_R,RED_D,RED_L,GRN_U,GRN_R,GRN_D,GRN_L,BLU_U,BLU_R,BLU_D,BLU_L,
		   WHT_U,  WHT_R,WHT_D,WHT_L,ORA_U,ORA_R,ORA_D,ORA_L,YEL_U,YEL_R,YEL_D,YEL_L};

// These three arrays are "transition tables".  They describe how the orientation of a block
// changes when it is rotated.
// For example, a block oriented "RED_U" (Red with line up ) becomes "ORA_U" (Orange with line up) when rotated 
// about the X axis 

int  rotz[] = {RED_R,RED_D,RED_L,RED_U,GRN_R,GRN_D,GRN_L,GRN_U,BLU_R,BLU_D,BLU_L,BLU_U,
	       WHT_R,WHT_D,WHT_L,WHT_U,ORA_R,ORA_D,ORA_L,ORA_U,YEL_R,YEL_D,YEL_L,YEL_U};

int irotz[] = {RED_L,RED_U,RED_R,RED_D,GRN_L,GRN_U,GRN_R,GRN_D,BLU_L,BLU_U,BLU_R,BLU_D,
	       WHT_L,WHT_U,WHT_R,WHT_D,ORA_L,ORA_U,ORA_R,ORA_D,YEL_L,YEL_U,YEL_R,YEL_D};

int  roty[] = {GRN_U,YEL_R,WHT_D,ORA_L,BLU_U,YEL_D,RED_D,ORA_D,WHT_U,YEL_L,GRN_D,ORA_R,
	       RED_U,YEL_U,BLU_D,ORA_U,GRN_R,RED_R,WHT_R,BLU_R,GRN_L,BLU_L,WHT_L,RED_L};

int iroty[] = {WHT_U,ORA_R,GRN_D,YEL_L,RED_U,ORA_U,BLU_D,YEL_U,GRN_U,ORA_L,WHT_D,YEL_R,
	       BLU_U,ORA_D,RED_D,YEL_D,WHT_L,BLU_L,GRN_L,RED_L,WHT_R,RED_R,GRN_R,BLU_R};

int  rotx[] = {ORA_U,GRN_R,YEL_D,WHT_L,ORA_L,BLU_R,YEL_L,RED_L,ORA_D,WHT_R,YEL_U,GRN_L,
	       ORA_R,RED_R,YEL_R,BLU_L,BLU_D,GRN_D,RED_D,WHT_D,RED_U,GRN_U,BLU_U,WHT_U};

int irotx[] = {YEL_U,WHT_R,ORA_D,GRN_L,YEL_R,RED_R,ORA_R,BLU_L,YEL_D,GRN_R,ORA_U,WHT_L,
	       YEL_L,BLU_R,ORA_L,RED_L,RED_U,WHT_U,BLU_U,GRN_U,BLU_D,WHT_D,RED_D,GRN_D};



// findFace finds the best match between the RotateCube matrix and the matricies in faceMatch array.
// There are four entries for each face to cover the four orientations of the face. 

int findFace(void)
{
    int base,xx,yy,n,maxbase;
    float total,max;

    max = -100.0;
    maxbase = 0;


    for(base = 0; base < 216; base += 9)
    {
	total = 0.0;
	n = 0;
	    
	    for(yy=0;yy<=2;yy++)
	    {

		for(xx=0;xx<=2;xx++)
		{
		    total += faceMatch[base+n] * RotateCube.elements[xx][yy];
		    n += 1;
		}
	    }

	    if(total > max)
	    {
		max = total;
		maxbase = base;
	    }
    }
    return(maxbase/36);
}



int loadPNG(const char *filename)
{
    GLuint texture;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep *row_pointers = NULL;
    int bitDepth, colourType;

    FILE *pngFile = fopen(filename, "rb");

    if (!pngFile)
        return 0;

    png_byte sig[8];

    fread(&sig, 8, sizeof(png_byte), pngFile);
    rewind(pngFile);
    if (!png_check_sig(sig, 8)) {
        printf("png sig failure\n");
        return 0;
    }

    png_ptr =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
        printf("png ptr not created\n");
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("set jmp failed\n");
        return 0;
    }

    info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr) {
        printf("cant get png info ptr\n");
        return 0;
    }

    png_init_io(png_ptr, pngFile);
    png_read_info(png_ptr, info_ptr);
    bitDepth = png_get_bit_depth(png_ptr, info_ptr);
    colourType = png_get_color_type(png_ptr, info_ptr);

    if (colourType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (colourType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
        //png_set_gray_1_2_4_to_8(png_ptr);
        png_set_expand_gray_1_2_4_to_8(png_ptr);  // thanks to Jesse Jaara for bug fix for newer libpng...
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if (bitDepth == 16)
        png_set_strip_16(png_ptr);
    else if (bitDepth < 8)
        png_set_packing(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    png_get_IHDR(png_ptr, info_ptr, &width, &height,
                 &bitDepth, &colourType, NULL, NULL, NULL);

    int components;		// = GetTextureInfo(colourType);
    switch (colourType) {
    case PNG_COLOR_TYPE_GRAY:
        components = 1;
        break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        components = 2;
        break;
    case PNG_COLOR_TYPE_RGB:
        components = 3;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        components = 4;
        break;
    default:
        components = -1;
    }

    if (components == -1) {
        if (png_ptr)
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        printf("%s broken?\n", filename);
        return 0;
    }

    GLubyte *pixels =
        (GLubyte *) malloc(sizeof(GLubyte) * (width * height * components));
    row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);

    int i;
    for (i = 0; i < height; ++i)
        row_pointers[i] =
            (png_bytep) (pixels + (i * width * components));

    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, NULL);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLuint glcolours;
    (components == 4) ? (glcolours = GL_RGBA) : (0);
    (components == 3) ? (glcolours = GL_RGB) : (0);
    (components == 2) ? (glcolours = GL_LUMINANCE_ALPHA) : (0);
    (components == 1) ? (glcolours = GL_LUMINANCE) : (0);

    //printf("%s has %i colour components\n",filename,components);
    //glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, glcolours, GL_UNSIGNED_BYTE, pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, glcolours, width, height, 0, glcolours,
                 GL_UNSIGNED_BYTE, pixels);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    fclose(pngFile);
    free(row_pointers);
    free(pixels);

    return texture;

}

// Setup things that don't change from frame to frame
// May get called to reset the cube to initial position
gboolean InitScene(void)
{
    int xx,yy,zz;
    static bool first = true;
    // Set all cubes to the initial orientation
    for(xx=0;xx<=2;xx+=1)
	for(yy=0;yy<=2;yy+=1)
	    for(zz=0;zz<=2;zz+=1)
		cube[xx][yy][zz].orientation = 0;

    // Set the inital value of the user controled cube rotation 
    identityToMatrix(&RotateCube);

    if(first)
    {
	first = false;

	//InitMouse();

	// Load the triangle shaders
	trianglesTextureProg = loadProgram("vertexRubikShader.c","fragmentRubikShader.c");
    
	// Get locations of shader parameters
	trianglesTexturePositionLoc = glGetAttribLocation(trianglesTextureProg, "a_position" );
	ModelViewProjectionMatrixLocation = glGetUniformLocation(trianglesTextureProg, "u_MVPMatrix" );
	MoveMatrixLocation = glGetUniformLocation(trianglesTextureProg, "u_moveMatrix" );
	RotateFaceMatrixLocation = glGetUniformLocation(trianglesTextureProg, "u_rotateFaceMatrix" );
	RotateBlockMatrixLocation = glGetUniformLocation(trianglesTextureProg, "u_rotateBlockMatrix" );
	TextureTriangleTextureLocation = glGetUniformLocation(trianglesTextureProg, "s_texture"); 
	TextureTriangleTexCoordLoc = glGetAttribLocation (trianglesTextureProg , "a_texCoord" );
	RotateMatrixLocation = glGetUniformLocation( trianglesTextureProg, "u_rotateMatrix" );

	// Print the locations just to check none are -1 which is an error.
	printf("%d %d %d %d %d %d %d %d\n",
	       trianglesTextureProg,
	       trianglesTexturePositionLoc,
	       ModelViewProjectionMatrixLocation,
	       MoveMatrixLocation,
	       RotateFaceMatrixLocation,
	       RotateBlockMatrixLocation,
	       TextureTriangleTextureLocation,
	       RotateMatrixLocation	);



	// Load the texture with the 6 coloured dots in it.
	trianglesTextureTexture = loadPNG("cube.png");

	// Calculate the Perspective transformation matrix
	perspectiveView(&PerspectiveMatrix,windowWidth,windowHeight,
			30.0f,1.0,12.0);

	// Set the background to dark gray.
	glClearColor ( 0.1f, 0.1f, 0.1f, 1.0f );

 


	// These can be done once as they never change because the cube is rotated rather than the camera moved.

	identityToMatrix(&ModelViewMatrix);
	// Move the camera away from the origin 
	moveCamera(&ModelViewMatrix,Camera1.x,Camera1.y,Camera1.z);

	// Form the MVP matrix.
	multiplyMatrix(&ModelViewProjectionMatrix,&ModelViewMatrix,&PerspectiveMatrix);
    }
    return TRUE;
}

// This is called once per frame. It is called before Draw.
// Change global things in here, like moving the camera ......
void Update(void)
{
    float dTheta,dPhi;

    // Scale the mouse movements
    dTheta = (mouseMoveLR / 50.0) ; 
    dPhi = (mouseMoveUD / 50.0) ;
 
    // Change cube orientation by rotating the RotateCube matrix
    Rotate(&RotateCube,dTheta,0.0,1.0,0.0);
    Rotate(&RotateCube,dPhi,1.0,0.0,0.0);
}

extern GLfloat IdentityMatrix[4][4];


// Called once per frame.  Use openGL calls etc to draw the scene.
void Draw(void)
{
 
    MATRIX_4x4 RotateMat;
    static MATRIX_4x4 rotations[24];
    int xx,yy,zz;
    static float faceRotationAngle = 0.0;

 
    static int face = 0;
    static bool cw = true;
    bool reverse;
    static bool rotating = false;
    static bool rotationDone = false;
    static int xxyyzz = 1;

    // Setup various options 
    glViewport(0,0,windowWidth,windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
    glEnable(GL_DEPTH_TEST);
    glDepthRangef(-5.0f,5.0f); 
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Set up back face (clockwise) culling
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    

    // Select the triangle drawing shader programme
    glUseProgram(trianglesTextureProg);
 
    // Tell the programme to use the verticies in array "Vertices"  
    glVertexAttribPointer(trianglesTexturePositionLoc,3,GL_FLOAT,GL_FALSE,0,Vertices);
    glEnableVertexAttribArray(trianglesTexturePositionLoc);

    // Tell the programme to use the texture coordiantes in array "Texture"  
    glVertexAttribPointer ( TextureTriangleTexCoordLoc, 2, GL_FLOAT,
			    GL_FALSE, 2 * sizeof(GLfloat), Texture );
    glEnableVertexAttribArray ( TextureTriangleTexCoordLoc );

    // Load the MVP matrix
    glUniformMatrix4fv( ModelViewProjectionMatrixLocation, 1, GL_FALSE, 
			(GLfloat*) &ModelViewProjectionMatrix.elements[0][0] );




    // Set up the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,trianglesTextureTexture);
    glUniform1i(TextureTriangleTextureLocation,0);

    // Check for mouse button presses when not already rotating a face
    if((mousePressLeft || mousePressRight) && !rotating )
    {
	int f;
	f = findFace();
	switch(f)
	{
	case 0:
	    face = 1;
	    reverse = false;
	    xxyyzz = 2;
	    rotating = true;
	    break;
	case 1:
	    face = 3;
	    reverse = true;
	    xxyyzz = 0;
	    rotating = true;
	    break;
	case 2:
	    face = 1;
	    reverse = true;
	    xxyyzz = 0;
	    rotating = true;
	    break;
	case 3:
	    face = 3;
	    reverse = false;
	    xxyyzz = 2;
	    rotating = true;
	    break;
	    
	case 4:
	    face = 2;
	    reverse = false;
	    xxyyzz = 2;
	    rotating = true;
	    break;
	case 5:
	    face = 2;
	    reverse = true;
	    xxyyzz = 0;
	    rotating = true;
	    break;



	default:
	    break;
	    
	}

	if(mousePressLeft) cw = true;
	if(mousePressRight) cw = false;

	if(reverse) cw = !cw;


    }
    mousePressLeft = mousePressRight = false;
  
    if(rotating)
    {
	if(cw)
	    faceRotationAngle += 1.0;
	else
	    faceRotationAngle -= 1.0;

	// Stop face rotation after turning through 90 degrees
	if((faceRotationAngle >= 90.0) || (faceRotationAngle <= -90.0))
	{
	    rotating = false;
	    rotationDone = true;
	    faceRotationAngle = 0.0;
	}
    }

    

    // When face rotation is complete, work out the new orientations of the cubes in the rotated face.
    if(rotationDone)
    {
	switch(face)
	{
	case 1:
	    if(cw)
	    {
		for(xx = 0; xx <= 2; xx++)
		{
		    for(yy = 0; yy <= 2; yy++)
		    {
			// Lookup the new orientation of each cube in the rotated face
			cube2[xx][yy][xxyyzz].orientation = rotz[cube[xx][yy][xxyyzz].orientation];
		    }
		}
		for(xx = 0; xx <= 2; xx++)
		{
		    for(yy = 0; yy <= 2; yy++)
		    {
			// Rotate the cubes in the face through 90degrees
			cube[yy][2-xx][xxyyzz].orientation = cube2[xx][yy][xxyyzz].orientation;
		    }
		}
	    }
	    else
	    {
		for(xx = 0; xx <= 2; xx++)
		{
		    for(yy = 0; yy <= 2; yy++)
		    {
			// Lookup the new orientation of each cube in the rotated face
			cube2[xx][yy][xxyyzz].orientation = irotz[cube[xx][yy][xxyyzz].orientation];
		    }
		}
		for(xx = 0; xx <= 2; xx++)
		{
		    for(yy = 0; yy <= 2; yy++)
		    {
			// Rotate the cubes through 90degrees
			cube[2-yy][xx][xxyyzz].orientation = cube2[xx][yy][xxyyzz].orientation;
		    }
		}
	    }
	    break;
	case 2:
	    if(cw)
	    {
		for(xx = 0; xx <= 2; xx++)
		{
		    for(zz = 0; zz <= 2; zz++)
		    {
			cube2[xx][xxyyzz][zz].orientation = roty[cube[xx][xxyyzz][zz].orientation];
		    }
		}
		for(xx = 0; xx <= 2; xx++)
		{
		    for(zz = 0; zz <= 2; zz++)
		    {
			cube[2-zz][xxyyzz][xx].orientation = cube2[xx][xxyyzz][zz].orientation;
		    }
		}
	    }
	    else
	    {
		for(xx = 0; xx <= 2; xx++)
		{
		    for(zz = 0; zz <= 2; zz++)
		    {
			cube2[xx][xxyyzz][zz].orientation = iroty[cube[xx][xxyyzz][zz].orientation];
		    }
		}
		for(xx = 0; xx <= 2; xx++)
		{
		    for(zz = 0; zz <= 2; zz++)
		    {
			cube[zz][xxyyzz][2-xx].orientation = cube2[xx][xxyyzz][zz].orientation;
		    }
		}
	    }
	    break;
	case 3:
	    if(cw)
	    {
		for(yy = 0; yy <= 2; yy++)
		{
		    for(zz = 0; zz <= 2; zz++)
		    {
			cube2[xxyyzz][yy][zz].orientation = rotx[cube[xxyyzz][yy][zz].orientation];
		    }
		}
		for(yy = 0; yy <= 2; yy++)
		{
		    for(zz = 0; zz <= 2; zz++)
		    {
			cube[xxyyzz][zz][2-yy].orientation = cube2[xxyyzz][yy][zz].orientation;
		    }
		}
	    }
	    else
	    {
		for(yy = 0; yy <= 2; yy++)
		{
		    for(zz = 0; zz <= 2; zz++)
		    {
			cube2[xxyyzz][yy][zz].orientation = irotx[cube[xxyyzz][yy][zz].orientation];
		    }
		}
		for(yy = 0; yy <= 2; yy++)
		{
		    for(zz = 0; zz <= 2; zz++)
		    {
			cube[xxyyzz][2-zz][yy].orientation = cube2[xxyyzz][yy][zz].orientation;
		    }
		}
	    }
	    break;
	}
	face = 0;
	rotationDone = false;
    }




    // Draw each block in the cube
    for(xx = 0; xx <= 2; xx++)
    {
	for(yy = 0; yy <= 2; yy++)
	{
	    for(zz = 0; zz <= 2; zz++)
	    {
		// Skip the middle block as it is never visible
		if((xx == 1) && (yy == 1) && (zz == 1)) continue;
		// Clear the rotation matrix
		identityToMatrix(&RotateMat);

		// When face != 0 a whole face is being rotated using u_rotateFaceMatrix in the vertex shader
		switch(face)
		{
		case 1:
		    if(zz == xxyyzz) Rotate(&RotateMat,faceRotationAngle,0.0,0.0,1.0);
		    break;
		case 2:
		    if(yy == xxyyzz) Rotate(&RotateMat,faceRotationAngle,0.0,1.0,0.0);
		    break;
		case 3:
		    if(xx == xxyyzz) Rotate(&RotateMat,faceRotationAngle,1.0,0.0,0.0);
		    break;
		default:
		    break;
		}
		// load RotateMat into the RotateMatrix in the shaders 
		glUniformMatrix4fv( RotateFaceMatrixLocation, 1, GL_FALSE, 
				    (GLfloat*) &RotateMat.elements[0][0] );

	
		
		// Build a precomputed array of rotation matricies.....
		// Rotate each block to the correct orientation using u_rotateBlockMatrix in the vertex shader
		// This uses a combination of rotations around only x and z axis
		// Don't forget the order of the Rotate calls is the reverse order to the rotations.
		{
		    static bool build = true;
		    if(build)
		    {
			int n;
			build = false;
			for(n=0;n<=23;n++)
			{
			    // Clear the rotation matrix
			    identityToMatrix(&RotateMat);
			    // This used to be switch(cube[xx][yy][zz].orientation)
			    switch(n)
			    {
				// Face 0
			    case 0:
				break;
			    case 1:
				Rotate(&RotateMat,90.0,0.0,0.0,1.0);
				break;
			    case 2:
				Rotate(&RotateMat,180.0,0.0,0.0,1.0);
				break;
			    case 3:
				Rotate(&RotateMat,270.0,0.0,0.0,1.0);
				break;
				// Face 1
			    case 4:
				Rotate(&RotateMat,90.0,0.0,1.0,0.0);
				break;
			    case 5:
				Rotate(&RotateMat,90.0,0.0,0.0,1.0);
				Rotate(&RotateMat,90.0,0.0,1.0,0.0);
				break;
			    case 6:
				Rotate(&RotateMat,180.0,0.0,0.0,1.0);
				Rotate(&RotateMat,90.0,0.0,1.0,0.0);
				break;
			    case 7:
				Rotate(&RotateMat,270.0,0.0,0.0,1.0);
				Rotate(&RotateMat,90.0,0.0,1.0,0.0);
				break;
				// Face 2
			    case 8:
				Rotate(&RotateMat,180.0,0.0,1.0,0.0);
				break;
			    case 9:
				Rotate(&RotateMat,90.0,0.0,0.0,1.0);
				Rotate(&RotateMat,180.0,0.0,1.0,0.0);
				break;
			    case 10:
				Rotate(&RotateMat,180.0,0.0,0.0,1.0);
				Rotate(&RotateMat,180.0,0.0,1.0,0.0);
				break;
			    case 11:
				Rotate(&RotateMat,270.0,0.0,0.0,1.0);
				Rotate(&RotateMat,180.0,0.0,1.0,0.0);
				break;
				// Face 3
			    case 12:
				Rotate(&RotateMat,270.0,0.0,1.0,0.0);
				break;
			    case 13:
				Rotate(&RotateMat,90.0,0.0,0.0,1.0);
				Rotate(&RotateMat,270.0,0.0,1.0,0.0);
	      
				break;
			    case 14:
				Rotate(&RotateMat,180.0,0.0,0.0,1.0);
				Rotate(&RotateMat,270.0,0.0,1.0,0.0);
				break;
			    case 15:
				Rotate(&RotateMat,270.0,0.0,0.0,1.0);
				Rotate(&RotateMat,270.0,0.0,1.0,0.0);
				break;
				// Face 4
			    case 16:
				Rotate(&RotateMat,90.0,1.0,0.0,0.0);
				break;
			    case 17:
				Rotate(&RotateMat,90.0,0.0,0.0,1.0);
				Rotate(&RotateMat,90.0,1.0,0.0,0.0);
				break;
			    case 18:
				Rotate(&RotateMat,180.0,0.0,0.0,1.0);
				Rotate(&RotateMat,90.0,1.0,0.0,0.0);
				break;
			    case 19:
				Rotate(&RotateMat,270.0,0.0,0.0,1.0);
				Rotate(&RotateMat,90.0,1.0,0.0,0.0);
				break;
				// Face 5
			    case 20:
				Rotate(&RotateMat,-90.0,1.0,0.0,0.0);
				break;
			    case 21:
				Rotate(&RotateMat,90.0,0.0,0.0,1.0);
				Rotate(&RotateMat,-90.0,1.0,0.0,0.0);
				break;
			    case 22:
				Rotate(&RotateMat,180.0,0.0,0.0,1.0);
				Rotate(&RotateMat,-90.0,1.0,0.0,0.0);
				break;
			    case 23:
				Rotate(&RotateMat,270.0,0.0,0.0,1.0);
				Rotate(&RotateMat,-90.0,1.0,0.0,0.0);
				break;
			    }
			    // Save the matrix in the rotations array
			    rotations[n] = RotateMat;
			}
		    }
		   
		}

		// Get the rotation matrix from the array
		RotateMat = rotations[cube[xx][yy][zz].orientation];
		
		// load RotateMat into the u_rotateBlockMatrix in the shaders 
		glUniformMatrix4fv( RotateBlockMatrixLocation, 1, GL_FALSE, 
				    (GLfloat*) &RotateMat.elements[0][0] );

		glUniformMatrix4fv( RotateMatrixLocation, 1, GL_FALSE, 
				    (GLfloat*) &RotateCube.elements[0][0] );

		// Move the block to its correct location  
		glUniform4f(MoveMatrixLocation,xx-1.0,yy-1.0,zz-1.0,0.0);

		// Draw a block
		glDrawElements ( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, Indices );

	    }
	}
    }
}

