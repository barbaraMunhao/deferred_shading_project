#version 420 core

 uniform vec4 vp[] = vec4[4](
    vec4(-1.0f, -1.0f, 0, 1),
    vec4(+1.0f, -1.0f, 0, 1),
    vec4(+1.0f, +1.0f, 0, 1),
    vec4(-1.0f, +1.0f, 0, 1));
    uniform vec2 uv[] = vec2[4](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f));
  
  out vec2 tx_coord;
  
 void main()
  {
    gl_Position = vp[gl_VertexID];
    tx_coord = uv[gl_VertexID];
 }