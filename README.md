# OpenGLSandboxShaderParser

This repository contains my custom shader parser, which takes in my custom GLSL supersest and generates valid GLSL code.

### features
- [x] shader splitting
- [x] parameters for material properties
- [ ] parameter display types (e.g. `#pragma display Color` or `#pragma display Slider 0.0 1.0`)

### example

input:
```glsl
#pragma version 460 core

#region parameters
	uniform vec3 u_Color;
	uniform float u_Ambient;
	uniform vec3 u_SunPos;
#endregion

#shader vertex
	layout(location = 0) in vec3 a_Position;
	layout(location = 1) in vec3 a_Normal;
	layout(location = 2) in vec2 a_TexCoord;

	uniform mat4 u_Model;
	uniform mat4 u_ViewProjection;

	void main()
	{
		gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
	}
#endshader

#shader fragment
	layout(location = 0) out vec4 fragColor;

	void main()
	{
		vec3 finalColor = u_Color * u_Ambient;
		fragColor = vec4(finalColor, 1.0);
	}
#endshader
```

outputs:

vertex
```glsl
#version 460 core
	uniform vec3 u_Color;
	uniform float u_Ambient;
	uniform vec3 u_SunPos;

	layout(location = 0) in vec3 a_Position;
	layout(location = 1) in vec3 a_Normal;
	layout(location = 2) in vec2 a_TexCoord;

	uniform mat4 u_Model;
	uniform mat4 u_ViewProjection;

	void main()
	{
		gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
	}
```

fragment
```glsl
#version 460 core
	uniform vec3 u_Color;
	uniform float u_Ambient;
	uniform vec3 u_SunPos;

	layout(location = 0) out vec4 fragColor;

	void main()
	{
		vec3 finalColor = u_Color * u_Ambient;
		fragColor = vec4(finalColor, 1.0);
	}
```
