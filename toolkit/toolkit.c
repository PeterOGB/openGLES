
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include "toolkit.h"

#define check() assert(glGetError() == 0)

EGLDisplay eglDisplay;
EGLSurface eglSurface;
EGLContext eglContext;
int windowWidth,windowHeight;

static int drawStuff(__attribute__((unused)) gpointer data);





GLfloat IdentityMatrix[4][4] = {
    { 1.0f,0.0f,0.0f,0.0f},
    { 0.0f,1.0f,0.0f,0.0f},
    { 0.0f,0.0f,1.0f,0.0f},
    { 0.0f,0.0f,0.0f,1.0f}
} ;

// Clear a matrix to the "do nothing" value
void identityToMatrix(MATRIX_4x4 *mat)
{
    memcpy(&mat->elements[0][0],&IdentityMatrix[0][0],sizeof(MATRIX_4x4));
}
// Multiply 2 4x4 matricies together.  product = A * B
void multiplyMatrix(MATRIX_4x4 *product,MATRIX_4x4 *A,MATRIX_4x4 *B)
{
    int z;
    GLfloat a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
    a = B->elements[0][0];
    b = B->elements[1][0];
    c = B->elements[2][0];
    d = B->elements[3][0];
    e = B->elements[0][1];
    f = B->elements[1][1];
    g = B->elements[2][1];
    h = B->elements[3][1];
    i = B->elements[0][2];
    j = B->elements[1][2];
    k = B->elements[2][2];
    l = B->elements[3][2];
    m = B->elements[0][3];
    n = B->elements[1][3];
    o = B->elements[2][3];
    p = B->elements[3][3];
    

    for (z=0; z<4; z++)
    {
	product->elements[z][0] =	
	    (A->elements[z][0] * a) +
	    (A->elements[z][1] * b) +
	    (A->elements[z][2] * c) +
	    (A->elements[z][3] * d) ;

	product->elements[z][1] =	
	    (A->elements[z][0] * e) + 
	    (A->elements[z][1] * f) +
	    (A->elements[z][2] * g) +
	    (A->elements[z][3] * h) ;

	product->elements[z][2] =
	    (A->elements[z][0] * i) + 
	    (A->elements[z][1] * j) +
	    (A->elements[z][2] * k) +
	    (A->elements[z][3] * l) ;

	product->elements[z][3] =	
	    (A->elements[z][0] * m) + 
	    (A->elements[z][1] * n) +
	    (A->elements[z][2] * o) +
	    (A->elements[z][3] * p) ;
    }
}


// Rotate the supplied matrix "angle" degrees about the axis (x,y,z) 
void Rotate(MATRIX_4x4 *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
   GLfloat sinAngle, cosAngle;
   GLfloat mag = sqrtf(x * x + y * y + z * z);
      
   sinAngle = sinf ( angle * (float)M_PI / 180.0f );
   cosAngle = cosf ( angle * (float)M_PI / 180.0f );
   if ( mag > 0.0f )
   {
      GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
      GLfloat oneMinusCos;
      MATRIX_4x4 rotMat;
   
      x /= mag;
      y /= mag;
      z /= mag;

      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;

      rotMat.elements[0][0] = (oneMinusCos * xx) + cosAngle;
      rotMat.elements[0][1] = (oneMinusCos * xy) - zs;
      rotMat.elements[0][2] = (oneMinusCos * zx) + ys;
      rotMat.elements[0][3] = 0.0F; 

      rotMat.elements[1][0] = (oneMinusCos * xy) + zs;
      rotMat.elements[1][1] = (oneMinusCos * yy) + cosAngle;
      rotMat.elements[1][2] = (oneMinusCos * yz) - xs;
      rotMat.elements[1][3] = 0.0F;

      rotMat.elements[2][0] = (oneMinusCos * zx) - ys;
      rotMat.elements[2][1] = (oneMinusCos * yz) + xs;
      rotMat.elements[2][2] = (oneMinusCos * zz) + cosAngle;
      rotMat.elements[2][3] = 0.0F; 

      rotMat.elements[3][0] = 0.0F;
      rotMat.elements[3][1] = 0.0F;
      rotMat.elements[3][2] = 0.0F;
      rotMat.elements[3][3] = 1.0F;

      multiplyMatrix( result, &rotMat, result );
   }
}

// Form the perspective transformation matrix from the supplied parameters.
void perspectiveView(MATRIX_4x4 *mat,
		     int screenWidth,int screenHeight,
		     GLfloat verticalFieldViewAngle,
		     GLfloat nearest,GLfloat farthest)
{
    GLfloat frustrumWidth,frustrumHeight;
    

    frustrumHeight = tanf(verticalFieldViewAngle/360.0f*(float)M_PI) * nearest;
    frustrumWidth =  frustrumHeight * (GLfloat) screenWidth / (GLfloat) screenHeight;

    identityToMatrix(mat);

    mat->elements[0][0] = nearest / frustrumWidth;

    mat->elements[1][1] = nearest / frustrumHeight;

    mat->elements[2][2] = -(nearest + farthest) / (farthest - nearest); 
    mat->elements[2][3] = -1.0f;

    mat->elements[3][2] = (-2.0f * nearest * farthest) / (farthest - nearest);
    mat->elements[3][3] = 0.0f;
}
		 
// Move the position of the camera (mat should be the ModelViewMatrix.
void moveCamera(MATRIX_4x4 *mat,GLfloat xpos,GLfloat ypos,GLfloat zpos)
{
    mat->elements[3][0] += (mat->elements[0][0] * xpos + 
			    mat->elements[1][0] * ypos + 
			    mat->elements[2][0] * zpos);
    mat->elements[3][1] += (mat->elements[0][1] * xpos + 
			    mat->elements[1][1] * ypos + 
			    mat->elements[2][1] * zpos);
    mat->elements[3][2] += (mat->elements[0][2] * xpos + 
			    mat->elements[1][2] * ypos + 
			    mat->elements[2][2] * zpos);
    mat->elements[3][3] += (mat->elements[0][3] * xpos + 
			    mat->elements[1][3] * ypos + 
			    mat->elements[2][3] * zpos);

}

void on_mainWindow_realize(__attribute__((unused)) GtkWidget *widget,
				__attribute__((unused)) gpointer data)
{
       EGLBoolean result;
    EGLint num_config;
    GdkWindow *gdkWindow;
    Window xWindow;

    static const EGLint attribute_list[] =
	{
	    EGL_RED_SIZE, 8,
	    EGL_GREEN_SIZE, 8,
	    EGL_BLUE_SIZE, 8,
	    EGL_ALPHA_SIZE, 8,
	    EGL_DEPTH_SIZE,16,
	    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	    EGL_NONE
	};

    static const EGLint context_attributes[] = 
	{
	    EGL_CONTEXT_CLIENT_VERSION, 3,
	    EGL_NONE
	};
    EGLConfig config;
    
    printf("%s called\n",__FUNCTION__);
    
    // Recover the xwindows window id from the gtk widget
    gdkWindow = gtk_widget_get_window (widget);
    xWindow = gdk_x11_window_get_xid (gdkWindow);
    
    
    // get an EGL display connection
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(eglDisplay != EGL_NO_DISPLAY);
    check();

    // initialize the EGL display connection
    result = eglInitialize(eglDisplay, NULL, NULL);
    assert(EGL_FALSE != result);
    check();

    // get an appropriate EGL frame buffer configuration
    result = eglChooseConfig(eglDisplay, attribute_list, &config, 1, &num_config);
    assert(EGL_FALSE != result);
    check();

    // get an appropriate EGL frame buffer configuration
    result = eglBindAPI(EGL_OPENGL_ES_API);
    assert(EGL_FALSE != result);
    check();

    // create an EGL rendering context
    eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, context_attributes);
    assert(eglContext!=EGL_NO_CONTEXT);
    check();

    windowWidth = gtk_widget_get_allocated_width (widget);
    windowHeight = gtk_widget_get_allocated_height (widget);

    eglSurface = eglCreateWindowSurface( eglDisplay, config, xWindow, NULL );
    assert(eglSurface != EGL_NO_SURFACE);
    check();
    //eglSurfaceAttrib(eglDisplay,eglSurface,EGL_SWAP_BEHAVIOR,EGL_BUFFER_DESTROYED);
    // connect the context to the surface
    result = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    assert(EGL_FALSE != result);
    check();

    // Set background color and clear buffers
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClear( GL_COLOR_BUFFER_BIT );

    check();

    eglSwapInterval(eglDisplay,1);    // 1 = 60Hz   2 = 30Hz
 

    
    // Call drawStuff when the gtk event loop is idle
    g_idle_add(drawStuff,NULL);

    if(InitScene() == TRUE)
    {
	g_idle_add(drawStuff,NULL);
    }
    printf("DONE\n");
    return FALSE;
}



// Handle closingthe window    
void on_mainWindow_delete_event(__attribute__((unused)) GtkWidget *widget,
				__attribute__((unused)) gpointer data)
{
    gtk_main_quit();
}


// Use the window "map-event" handler to set up EGL and openGLES
// The map-event signal is emitted when the widget 's window is mapped.
// A window is mapped when it becomes visible on the screen.

gboolean on_mainWindow_map_event(GtkWidget *widget,
				 __attribute__((unused))GdkEvent  *event,
				 __attribute__((unused))gpointer   user_data);

gboolean on_mainWindow_map_event(GtkWidget *widget,
				 __attribute__((unused))GdkEvent  *event,
				 __attribute__((unused))gpointer   user_data)

{
        printf("%s\n",__FUNCTION__);
	return TRUE;
}
 

static int drawStuff(__attribute__((unused)) gpointer data)
{
    Update();
    
    Draw();
	glFlush();
        glFinish();
    eglSwapBuffers(eglDisplay,eglSurface);

    return  G_SOURCE_CONTINUE ;

}
// Load Vertex or Fragment shader code from a file.
static GLuint loadShader(const char *filename,GLenum VorF)
{
    int shaderFd;
    GLchar *shaderText;
    GLuint shaderId;
    GLint compiledOk;
    struct stat buf;

    shaderId = glCreateShader(VorF);
    if(shaderId == 0)
	return(0);

    shaderFd = open(filename,O_RDONLY);
    fstat(shaderFd,&buf);
    shaderText = (GLchar *) malloc((size_t)buf.st_size+1);

    read(shaderFd,shaderText,(size_t)buf.st_size);
    close(shaderFd);
    shaderText[buf.st_size] = '\0';

    glShaderSource(shaderId,1,(const GLchar * const *)&shaderText,NULL);

    free(shaderText);

    glCompileShader(shaderId);

    glGetShaderiv(shaderId,GL_COMPILE_STATUS,&compiledOk );

    if(compiledOk == GL_FALSE)
    {
	GLint logLength;

	glGetShaderiv(shaderId,GL_INFO_LOG_LENGTH,&logLength);
      
	if(logLength > 1)
	{
	    char *logText;
	    logText = malloc((size_t)logLength);

	    glGetShaderInfoLog(shaderId,logLength,NULL,logText);
	    fprintf(stderr,"Shader from %s didn't compile!\n%s\n",filename,logText);            
         
	    free(logText);
      }

      glDeleteShader(shaderId);
      return 0;
    }
    return(shaderId);
}

// Build a shader program from vertext and fragment shader source files. 
GLuint loadProgram(const char *vFilename,const char *fFilename)
{
   GLuint vShaderId;
   GLuint fShaderId;
   GLuint progId;
   GLint linkOk;
    
   vShaderId = loadShader(vFilename,GL_VERTEX_SHADER);
   if(vShaderId == 0) return(0);

   fShaderId = loadShader(fFilename,GL_FRAGMENT_SHADER);
   if(fShaderId == 0) 
   {
       glDeleteShader(vShaderId);
       return(0);
   }

   progId = glCreateProgram();
   if(progId == 0)
   {
       glDeleteShader(vShaderId);
       glDeleteShader(fShaderId);
       return(0);
   }   

   glAttachShader(progId,vShaderId);
   glAttachShader(progId,fShaderId);
   
   glLinkProgram(progId);

   glGetProgramiv(progId,GL_LINK_STATUS,&linkOk);

   if(linkOk == GL_FALSE)
   {
       GLint logLength;

       glGetProgramiv(progId,GL_INFO_LOG_LENGTH,&logLength);
      
       if(logLength > 1)
       {
	   char *logText;
	   logText = malloc((size_t)logLength);
	   
	   glGetProgramInfoLog(progId,logLength,NULL,logText);
	   fprintf(stderr,"Link failed: %s\n",logText);            
	   
	   free(logText);     
       }
       glDeleteProgram(progId);
       return(0);
   }
   glDeleteShader(vShaderId);
   glDeleteShader(fShaderId);

   return(progId);
}





int main ( int argc, char **argv) {

    GtkWidget *window;
    GtkBuilder *builder = NULL;

    
    gtk_init (&argc , &argv);

    // Create a builder
    builder = gtk_builder_new();

    // And read gui definition from a file
    if( gtk_builder_add_from_file (builder,"generic1080x1080.glade" , NULL) == 0)
    {
        printf("gtk_builder_add_from_file FAILED\n");
	return(0);
    }

    // Get a reference to the main window widget
    window  = GTK_WIDGET (gtk_builder_get_object (builder,"mainWindow"));
  

    gtk_builder_connect_signals(builder,NULL);

    // state->verbose = 1;
    
    gtk_widget_show_all (window);
    gtk_main ();


    // Tidy up EGL.
    eglDestroyContext ( eglDisplay, eglContext );
    eglDestroySurface ( eglDisplay, eglSurface );
    eglTerminate      ( eglDisplay );
    
    return 0;
}
