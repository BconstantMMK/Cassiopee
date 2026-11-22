#version 120
// GLSL 1.20 fallback: plain pass-through (no tessellation stage in GL 2.1)
void main(void)
{
  gl_Position = gl_Vertex;
}
