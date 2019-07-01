uniform mat4 u_MVPMatrix;
uniform vec4 u_moveMatrix;
uniform mat4 u_rotateMatrix;
//uniform vec4 u_triC;
attribute vec4 a_position;
//varying vec4 pos;

void main()
{
    //pos = a_position ; //u_triC;
    gl_Position = u_MVPMatrix * ((u_rotateMatrix * a_position) + u_moveMatrix);
}
