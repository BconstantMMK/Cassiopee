#version 120
// GLSL 1.20 fallback (no geometry shader): varyings feed the fragment shader directly
varying vec4 P0;
varying vec4 P1;
varying vec4 position;
varying vec4 normal;
varying vec4 color;
varying vec3 Nv;
varying vec3 P;
varying vec4 vert;
varying float gAlpha;
uniform int fix_length;
uniform float scale;

void main()
{
  gl_Position = ftransform();
  position = gl_Vertex;
  P0 = gl_Position;
  vec4 transpos = vec4(gl_Color.xyz,0.) - vec4(0.5,0.5,0.5,0.0);
  transpos = (1-fix_length)*transpos + fix_length*normalize(transpos);

  P1 = gl_ModelViewProjectionMatrix*(gl_Vertex+scale*vec4(transpos.xyz,0.));
  normal = vec4(gl_Normal,0.);
  color = gl_Color;
  // varyings expected by streamline.frag when no geometry shader is used
  vert = gl_Vertex;
  P = (gl_ModelViewMatrix * gl_Vertex).xyz;
  Nv = gl_NormalMatrix * gl_Normal;
  gAlpha = 0.;
}
