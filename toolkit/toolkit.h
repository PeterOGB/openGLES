
// Matrix Support

typedef struct
{
    GLfloat   elements[4][4];
} MATRIX_4x4;


typedef struct
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
} POINT;

extern int windowWidth,windowHeight;

void perspectiveView(MATRIX_4x4 *mat,
		     int screenWidth,int screenHeight,
		     GLfloat verticalFieldViewAngle,
		     GLfloat nearest,GLfloat farthest);

GLuint loadProgram(const char *vFilename,const char *fFilename);
void identityToMatrix(MATRIX_4x4 *mat);
void moveCamera(MATRIX_4x4 *mat,GLfloat xpos,GLfloat ypos,GLfloat zpos);
void Rotate(MATRIX_4x4 *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void multiplyMatrix(MATRIX_4x4 *product,MATRIX_4x4 *A,MATRIX_4x4 *B);

gboolean InitScene(void);
void Update(void);
void Draw(void);


