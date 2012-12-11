precision mediump float;
uniform vec4 vColor;
uniform sampler2D t_sampler;
varying vec2 v_TextureCoord;
void main() {
    gl_FragColor = vColor;
}
