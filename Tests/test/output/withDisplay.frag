#version 460 core
	uniform vec3 u_Color;	uniform float u_Ambient;	uniform vec3 u_SunPos;
	layout(location = 0) out vec4 fragColor;	void main()	{		vec3 finalColor = u_Color * u_Ambient;		fragColor = vec4(finalColor, 1.0);	}
