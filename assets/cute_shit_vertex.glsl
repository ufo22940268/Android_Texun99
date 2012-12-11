attribute vec4 a_Position;
attribute vec2 a_TexCoord;
uniform mat4 u_MVPMatrix;
varying vec2 v_TexCoord;
void main() {
    gl_Position = a_Position*u_MVPMatrix;
    v_TexCoord = a_TexCoord;
}
