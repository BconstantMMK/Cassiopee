#version 120
// GLSL 1.20 fallback (no geometry shader): varyings feed the fragment shader directly
varying vec4 P0;// Coordonnees dans l'espace projete ecran
varying vec4 vP;// Coordonnees dans l'espace camera
varying vec4 e3;
varying vec4 position;// Position dans l'espace world
varying vec4 normal;
varying vec4 color;
varying vec3 Nv;
varying vec3 P;

uniform int fix_length;
uniform float scale;

void main()
{
  gl_Position = ftransform();
  position = gl_Vertex;
  P0 = gl_Position;
  vP = gl_ModelViewMatrix * gl_Vertex;
  color = gl_Color;
  vec4 transpos = vec4(gl_Color.xyz,0.) - vec4(0.5,0.5,0.5,0.0);
  transpos = (1-fix_length)*transpos + 0.5*fix_length*normalize(transpos);
  normal = vec4(gl_Normal,0.);
  e3 = scale * vec4(transpos.xyz,0.);
  // varyings expected by streamarrow.frag when no geometry shader is used
  Nv = gl_NormalMatrix * gl_Normal;
  P = vP.xyz;
}
