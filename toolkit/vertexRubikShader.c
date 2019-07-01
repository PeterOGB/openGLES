uniform mat4 u_MVPMatrix;
uniform vec4 u_moveMatrix;
uniform mat4 u_rotateFaceMatrix;
uniform mat4 u_rotateBlockMatrix;
uniform mat4 u_rotateMatrix;
attribute vec4 a_position;
attribute vec2 a_texCoord;
varying vec2 v_texCoord;

void main()
{
    // u_rotateBlockMatrix orientates the block
    // u_moveMatrix moves the block to the correct position in the cube
    // u_rotateFaceMatrix is normally the identity, but rotates a face while it is moving
    // u_rotateMatrix rotates the whole cube under user control
    // u_MVPMatrix is the view-projection matrix
    gl_Position =  u_MVPMatrix * ((u_rotateFaceMatrix * ((u_rotateBlockMatrix * a_position) + u_moveMatrix))* u_rotateMatrix  );
    v_texCoord = a_texCoord;
}
