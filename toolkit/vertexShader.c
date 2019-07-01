uniform mediump mat4 u_MVPMatrix;
uniform mediump vec4 u_moveMatrix;
uniform mediump mat4 u_rotateMatrix;
attribute vec4 a_position;
void main()
{
    gl_Position = u_MVPMatrix * ((u_rotateMatrix * a_position) + u_moveMatrix);
}
