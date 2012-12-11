attribute vec4 vPosition;
attribute float vPointSize;
attribute vec2 a_TextureCoord;
uniform mat4 uMVPMatrix;
uniform mat4 uOthoMatrix;
varying vec2 v_TextureCoord;
void main() {
    gl_Position = vPosition*uMVPMatrix*uOthoMatrix;
    v_TextureCoord = a_TextureCoord;
    gl_PointSize = vPointSize;
}
