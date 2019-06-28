// Compile with 
// gcc -Wall -Wextra -o hello_triangle2 hello_triangle2.c -lEGL -lGLESv2 $(pkg-config gtk+-3.0 --cflags --libs) -rdynamic 
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include  <X11/Xlib.h>
#include "GLES2/gl2.h"
#include "EGL/egl.h"


#include <assert.h>

GLfloat cx,cy;

typedef struct
{
    uint32_t screen_width;
    uint32_t screen_height;
//  OpenGL|ES objects
    Window xWindow;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    GLuint verbose;
    GLuint vshader;
    GLuint fshader;
    GLuint mshader;
    GLuint program;
    GLuint program2;
    GLuint tex_fb;
    GLuint tex;
    GLuint buf;
//  julia attribs
    GLuint unif_color, attr_vertex, unif_scale, unif_offset, unif_tex, unif_centre; 
//  mandelbrot attribs
    GLuint attr_vertex2, unif_scale2, unif_offset2, unif_centre2;
} CUBE_STATE_T;

#define check() assert(glGetError() == 0)

static void showlog(GLint shader)
{
    // Prints the compile log for a shader
    char log[1024];
    glGetShaderInfoLog(shader,sizeof log,NULL,log);
    printf("%d:shader:\n%s\n", shader, log);
}

static void showprogramlog(GLint shader)
{
    // Prints the information log for a program object
    char log[1024];
    glGetProgramInfoLog(shader,sizeof log,NULL,log);
    printf("%d:program:\n%s\n", shader, log);
}

static CUBE_STATE_T _state, *state=&_state;


static void draw_mandelbrot_to_texture(CUBE_STATE_T *state, GLfloat cx,
				       GLfloat cy, GLfloat scale);

static void draw_triangles(CUBE_STATE_T *state, GLfloat cx, GLfloat cy,
			   GLfloat scale, GLfloat x, GLfloat y);

static void init_shaders(CUBE_STATE_T *state);


// Gtk main loop "idle handler" that runs the openGLES drawing code
static int drawStuff(__attribute__((unused)) gpointer data)
{
    static gboolean first = TRUE;
    if(first)
    {
	// Make the fixed texture
	draw_mandelbrot_to_texture(state, state->screen_width/2.0,
				   state->screen_height/2.0 ,0.003);
	first = FALSE;
    }
    draw_triangles(state,state->screen_width/2.0,
		   state->screen_height/2.0,0.003,cx,cy);
    return  G_SOURCE_CONTINUE ;

}

// Use mouse pointer motion events to update global variables
gboolean on_mainWindow_motion_notify_event (__attribute__((unused)) GtkWidget *widget,
					    GdkEventMotion  *event,
					    __attribute__((unused)) gpointer   user_data)
{
    cx = state->screen_width - event->x;
    cy = event->y;
    return FALSE;
}
    
// Use the window "map-event" handler to set up EGL and openGLES
// The map-event signal is emitted when the widget 's window is mapped.
// A window is mapped when it becomes visible on the screen.
gboolean on_mainWindow_map_event(GtkWidget *widget,
				 __attribute__((unused))GdkEvent  *event,
				 __attribute__((unused))gpointer   user_data)

{
    EGLBoolean result;
    EGLint num_config;
    GdkWindow *gdkWindow;
    Window win;

    static const EGLint attribute_list[] =
	{
	    EGL_RED_SIZE, 8,
	    EGL_GREEN_SIZE, 8,
	    EGL_BLUE_SIZE, 8,
	    EGL_ALPHA_SIZE, 8,
	    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	    EGL_NONE
	};

    static const EGLint context_attributes[] = 
	{
	    EGL_CONTEXT_CLIENT_VERSION, 2,
	    EGL_NONE
	};
    EGLConfig config;

    
    // Recover the xwindows window id from the gtk widget
    gdkWindow = gtk_widget_get_window (widget);
    win = gdk_x11_window_get_xid (gdkWindow);
    state->xWindow = win;
    
    // get an EGL display connection
    state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(state->display!=EGL_NO_DISPLAY);
    check();

    // initialize the EGL display connection
    result = eglInitialize(state->display, NULL, NULL);
    assert(EGL_FALSE != result);
    check();

    // get an appropriate EGL frame buffer configuration
    result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
    assert(EGL_FALSE != result);
    check();

    // get an appropriate EGL frame buffer configuration
    result = eglBindAPI(EGL_OPENGL_ES_API);
    assert(EGL_FALSE != result);
    check();

    // create an EGL rendering context
    state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
    assert(state->context!=EGL_NO_CONTEXT);
    check();

    state->screen_width = gtk_widget_get_allocated_width (widget);
    state->screen_height = gtk_widget_get_allocated_height (widget);

    state->surface = eglCreateWindowSurface( state->display, config, win, NULL );
    assert(state->surface != EGL_NO_SURFACE);
    check();

    // connect the context to the surface
    result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
    assert(EGL_FALSE != result);
    check();

    // Set background color and clear buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear( GL_COLOR_BUFFER_BIT );

    check();

    // Call drawStuff when the gtk event loop is idle
    g_idle_add(drawStuff,NULL);

    init_shaders(state);

    return FALSE;
}


// Handle closingthe window    
void on_mainWindow_delete_event(__attribute__((unused)) GtkWidget *widget,
				__attribute__((unused)) gpointer data)
{
    gtk_main_quit();
}


// Setup the shaders
static void init_shaders(CUBE_STATE_T *state)
{
   static const GLfloat vertex_data[] = {
        -1.0,-1.0,1.0,1.0,
        1.0,-1.0,1.0,1.0,
        1.0,1.0,1.0,1.0,
        -1.0,1.0,1.0,1.0
   };
   const GLchar *vshader_source =
              "attribute vec4 vertex;"
              "varying vec2 tcoord;"
              "void main(void) {"
              " vec4 pos = vertex;"
              " gl_Position = pos;"
              " tcoord = vertex.xy*0.5+0.5;"
              "}";
      
   //Mandelbrot
   const GLchar *mandelbrot_fshader_source =
"uniform mediump vec4 color;"
"uniform mediump vec2 scale;"
"uniform mediump vec2 centre;"
"varying mediump vec2 tcoord;"
"void main(void) {"
"  mediump float intensity;"
"  mediump vec4  color2;"
"  mediump float cr=(gl_FragCoord.x-centre.x)*scale.x;"
"  mediump float ci=(gl_FragCoord.y-centre.y)*scale.y;"
"  mediump float ar=cr;"
"  mediump float ai=ci;"
"  mediump float tr,ti;"
"  mediump float col=0.0;"
"  mediump float p=0.0;"
"  int i=0;"
"  for(int i2=1;i2<16;i2++)"
"  {"
"    tr=ar*ar-ai*ai+cr;"
"    ti=2.0*ar*ai+ci;"
"    p=tr*tr+ti*ti;"
"    ar=tr;"
"    ai=ti;"
"    if (p>16.0)"
"    {"
"      i=i2;"
"      break;"
"    }"
"  }"
"  color2 = vec4(float(i)*0.0625,0,0,1);"
"  gl_FragColor = color2;"
"}";

   // Julia
   const GLchar *julia_fshader_source =
"uniform mediump vec4 color;"
"uniform mediump vec2 scale;"
"uniform mediump vec2 centre;"
"uniform mediump vec2 offset;"
"varying mediump vec2 tcoord;"
"uniform sampler2D tex;"
"void main(void) {"
"  mediump float  intensity;"
"  mediump vec4 color2;"
"  mediump float   ar=(gl_FragCoord.x-centre.x)*scale.x;"
"  mediump float   ai=(gl_FragCoord.y-centre.y)*scale.y;"
"  mediump float   cr=(offset.x-centre.x)*scale.x;"
"  mediump float   ci=(offset.y-centre.y)*scale.y;"
"  mediump float   tr,ti;"
"  mediump float   col=0.0;"
"  mediump float   p=0.0;"
"  int i=0;"
"  mediump vec2 t2;"
"  t2.x=tcoord.x+(offset.x-centre.x)*(0.5/centre.y);"
"  t2.y=tcoord.y+(offset.y-centre.y)*(0.5/centre.x);"
"  for(int i2=1;i2<16;i2++)"
"  {"
"    tr=ar*ar-ai*ai+cr;"
"    ti=2.0*ar*ai+ci;"
"    p=tr*tr+ti*ti;"
"    ar=tr;"
"    ai=ti;"
"    if (p>16.0)"
"    {"
"      i=i2;"
"      break;"
"    }"
"  }"
"  color2 = vec4(0,float(i)*0.0625,0,1);"
"  color2 = color2+texture2D(tex,t2);"
"  gl_FragColor = color2;"
"}";

        state->vshader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(state->vshader, 1, &vshader_source, 0);
        glCompileShader(state->vshader);
        check();

        if (state->verbose)
            showlog(state->vshader);
            
        state->fshader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(state->fshader, 1, &julia_fshader_source, 0);
        glCompileShader(state->fshader);
        check();

        if (state->verbose)
            showlog(state->fshader);

        state->mshader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(state->mshader, 1, &mandelbrot_fshader_source, 0);
        glCompileShader(state->mshader);
        check();

        if (state->verbose)
            showlog(state->mshader);

        // julia 
        state->program = glCreateProgram();
        glAttachShader(state->program, state->vshader);
        glAttachShader(state->program, state->fshader);
        glLinkProgram(state->program);
        check();

        if (state->verbose)
            showprogramlog(state->program);
            
        state->attr_vertex = glGetAttribLocation(state->program, "vertex");
        state->unif_color  = glGetUniformLocation(state->program, "color");
        state->unif_scale  = glGetUniformLocation(state->program, "scale");
        state->unif_offset = glGetUniformLocation(state->program, "offset");
        state->unif_tex    = glGetUniformLocation(state->program, "tex");       
        state->unif_centre = glGetUniformLocation(state->program, "centre");

        // mandelbrot
        state->program2 = glCreateProgram();
        glAttachShader(state->program2, state->vshader);
        glAttachShader(state->program2, state->mshader);
        glLinkProgram(state->program2);
        check();

        if (state->verbose)
            showprogramlog(state->program2);
            
        state->attr_vertex2 = glGetAttribLocation(state->program2, "vertex");
        state->unif_scale2  = glGetUniformLocation(state->program2, "scale");
        state->unif_offset2 = glGetUniformLocation(state->program2, "offset");
        state->unif_centre2 = glGetUniformLocation(state->program2, "centre");
        check();
   
        glClearColor ( 0.0, 1.0, 1.0, 1.0 );
        
        glGenBuffers(1, &state->buf);

        check();

        // Prepare a texture image
        glGenTextures(1, &state->tex);
        check();
        glBindTexture(GL_TEXTURE_2D,state->tex);
        check();
        // glActiveTexture(0)
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,state->screen_width,state->screen_height,
		     0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,0);
        check();
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        check();
        // Prepare a framebuffer for rendering
        glGenFramebuffers(1,&state->tex_fb);
        check();
        glBindFramebuffer(GL_FRAMEBUFFER,state->tex_fb);
        check();
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,state->tex,0);
        check();
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        check();
        // Prepare viewport
        glViewport ( 0, 0, state->screen_width, state->screen_height );
        check();
        
        // Upload vertex data to a buffer
        glBindBuffer(GL_ARRAY_BUFFER, state->buf);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data),
                             vertex_data, GL_STATIC_DRAW);
        glVertexAttribPointer(state->attr_vertex, 4, GL_FLOAT, 0, 16, 0);
        glEnableVertexAttribArray(state->attr_vertex);
        glVertexAttribPointer(state->attr_vertex2, 4, GL_FLOAT, 0, 16, 0);
        glEnableVertexAttribArray(state->attr_vertex2);

        check();
}


static void draw_mandelbrot_to_texture(CUBE_STATE_T *state, GLfloat cx, GLfloat cy, GLfloat scale)
{
        // Draw the mandelbrot to a texture
        glBindFramebuffer(GL_FRAMEBUFFER,state->tex_fb);
        check();

        glBindBuffer(GL_ARRAY_BUFFER, state->buf);
	check();
        
        glUseProgram ( state->program2 );
        check();

	glUniform2f(state->unif_scale2, scale, scale);
        glUniform2f(state->unif_centre2, cx, cy);
        check();

	glDrawArrays ( GL_TRIANGLE_FAN, 0, 4 );
        check();

	glFlush();
        glFinish();
        check();

	eglSwapBuffers(state->display, state->surface);
        check();
}
        
static void draw_triangles(CUBE_STATE_T *state, GLfloat cx, GLfloat cy, GLfloat scale, GLfloat x, GLfloat y)
{
        // Now render to the main frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        // Clear the background (not really necessary I suppose)
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        check();
        
        glBindBuffer(GL_ARRAY_BUFFER, state->buf);
        check();
        glUseProgram ( state->program );
        check();
        glBindTexture(GL_TEXTURE_2D,state->tex);
        check();
        glUniform4f(state->unif_color, 0.5, 0.5, 0.8, 1.0);
        glUniform2f(state->unif_scale, scale, scale);
        glUniform2f(state->unif_offset, x, y);
        glUniform2f(state->unif_centre, cx, cy);
	// I don't really understand this part, perhaps it relates to active texture?
        glUniform1i(state->unif_tex, 0); 
        check();
        
        glDrawArrays ( GL_TRIANGLE_FAN, 0, 4 );
        check();

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glFlush();
        glFinish();
        check();
        
        eglSwapBuffers(state->display, state->surface);
        check();
}





int main ( int argc, char **argv) {

    GtkWidget *window;
    GtkBuilder *builder = NULL;

    
    gtk_init (&argc , &argv);

    // Create a builder
    builder = gtk_builder_new();

    // And read gui definition from a file
    if( gtk_builder_add_from_file (builder,"hello_triangle2.glade" , NULL) == 0)
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
    eglDestroyContext ( state->display, state->context );
    eglDestroySurface ( state->display, state->surface );
    eglTerminate      ( state->display );
    
    return 0;
}
