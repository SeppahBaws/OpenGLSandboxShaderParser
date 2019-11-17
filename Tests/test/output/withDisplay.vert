#version 460 core
	uniform vec3 u_Color;	uniform float u_Ambient;	uniform vec3 u_SunPos;
	layout(location = 0) in vec3 a_Position;	layout(location = 1) in vec3 a_Normal;	layout(location = 2) in vec2 a_TexCoord;	uniform mat4 u_Model;	uniform mat4 u_ViewProjection;	void main()	{		gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);	}
