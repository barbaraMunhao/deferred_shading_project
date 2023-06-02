#version 420 core

struct LightProps
{
  vec4 position; // light position in eye coordinates
  vec4 color; // light color
  int type; // 0 point; 1 directional
};
uniform sampler2D tex_diffuse;

uniform LightProps lights[10];
uniform int num_lights;
uniform vec4 Os;
uniform vec4 Od;
uniform float ns;
uniform int use_texture;

out vec4 fragmentColor;

in vec3 v_position;
in vec3 v_normal;
in vec2 tx_coord;


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
	vec4 color;

	if(use_texture == 1)
	{
		color = texture(tex_diffuse, tx_coord);
	}
	else
	{
		color = vec4(Od.xyz, 1);
	}

	fragmentColor = apply_phong(vec4(v_position, 0), v_normal, color, Os, ns);	
}