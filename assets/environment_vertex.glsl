attribute vec4 vPosition;
attribute float vPointSize;
attribute vec2 a_TextureCoord;
uniform mat4 u_ViewProjection;
uniform mat4 u_ModelProjection;
varying vec2 v_TextureCoord;
void main() {
    gl_Position = u_ModelProjection*u_ViewProjection*vPosition;
    v_TextureCoord = a_TextureCoord;
    gl_PointSize = vPointSize;
}
