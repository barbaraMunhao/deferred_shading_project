#version 420 core

uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform mat4 mvpMatrix;



layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;


out vec3 v_position;
out vec3 v_normal;
out vec2 tx_coord;



void main()
{
	v_position = vec3(mvMatrix * position);
	v_normal = normalize(normalMatrix * normal);
	tx_coord = uv;
	gl_Position = mvpMatrix * position;
}
