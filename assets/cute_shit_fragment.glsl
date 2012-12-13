precision mediump float;
uniform sampler2D u_sampler; 
uniform vec4 u_color; 
varying vec2 v_TexCoord;
void main() {
    /*gl_FragColor = u_color * texture2D(u_sampler, v_TexCoord);*/
    gl_FragColor = texture2D(u_sampler, v_TexCoord);
}
