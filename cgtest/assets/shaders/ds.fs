#version 420 core
layout (binding = 0) uniform usampler2D gbuf_odni; // uints
layout (binding = 1) uniform usampler2D gbuf_pns; // floats


struct LightProps
{
  vec4 position; // light position in eye coordinates
  vec4 color; // light color
  int type; // 0 point; 1 directional
};

struct fragment
{
	vec3 color;
	vec3 normal;
	float ns;
	vec3 coord;
	vec3 os;
};

uniform LightProps lights[10];
uniform int num_lights;
in vec2 tx_coord;
out vec4 fragmentColor;


void unpack_fragment(in vec2 coord, out fragment f)
{
	uvec4 d0 = textureLod(gbuf_odni, coord, 0);
	uvec4 d1 = textureLod(gbuf_pns, coord, 0);
	
	vec2 temp;
	temp = unpackHalf2x16(d0.y);
	
	f.color = vec3(unpackHalf2x16(d0.x), temp.x);
	f.normal = normalize(vec3(temp.y, unpackHalf2x16(d0.z)));

	f.coord = uintBitsToFloat(d1.xyz);

	vec2 osz_ns = unpackHalf2x16(d1.w);
	f.os = vec3(unpackHalf2x16(d0.w), osz_ns.x);
	f.ns = osz_ns.y;
}

vec4 apply_phong(in vec4 position, in vec3 normal, in vec4 Od, vec4 Os, in float ns)
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 Il;
	vec3 L, R;	
	vec3 V = normalize(position.xyz);

	int i;
	for(i = 0; i < num_lights; i++)
	{
		if(lights[i].type == 4) // directional
		{
			L = -lights[i].position.xyz;
			Il = lights[i].color;	
		}
		else // point
		{
			L = vec3(lights[i].position - position);
			Il = lights[i].color/ length(L);
		}

		L = normalize(L);
		R =  reflect(L, normal);
		color += Il * (Od * max(dot(normal, L), 0) + Os*pow(max(dot(R, V), 0), ns)); //specular
	}
	return color;
}


void main()
{
	fragment f;
	vec2 coord = gl_FragCoord.xy / textureSize(gbuf_odni, 0);
	unpack_fragment(tx_coord, f);
	fragmentColor = apply_phong(vec4(f.coord, 0), f.normal, vec4(f.color, 1), vec4(f.os, 1.0), f.ns);	
}