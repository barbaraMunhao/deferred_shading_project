#version 420 core

layout (location = 0) out uvec4 od_n_id;
layout (location = 1) out uvec4 pos_ns;

uniform sampler2D tex_diffuse;
uniform vec4 Os;
uniform vec4 Od;
uniform float ns;
uniform int use_texture;

in vec3 v_normal;
in vec2 tx_coord;
in vec3 v_position;

void main(void)
{
	uvec4 temp_od_n_id = uvec4(0);
	uvec4 temp_pos_ns = uvec4(0);
	vec4 f_color;
	if(use_texture == 1)
	{
		f_color = texture(tex_diffuse, tx_coord);
	}
	else
	{
		f_color = vec4(Od.xyz, 1);
	}
	
	temp_od_n_id.x = packHalf2x16(f_color.xy);
	temp_od_n_id.y = packHalf2x16(vec2(f_color.z, v_normal.x));
	temp_od_n_id.z = packHalf2x16(v_normal.yz);
	temp_od_n_id.w = packHalf2x16(Os.xy);

	temp_pos_ns.xyz = floatBitsToUint(v_position);
	temp_pos_ns.w = packHalf2x16(vec2(Os.z, ns));

	od_n_id = temp_od_n_id;
	pos_ns = temp_pos_ns;
}






























