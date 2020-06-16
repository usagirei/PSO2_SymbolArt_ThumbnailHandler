#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 proj;

out vec4 Color;
out vec2 TexCoords;
out vec3 Position;

void main()
{
    gl_Position = proj * view * vec4(aPos.x, aPos.y, aPos.z, 1.0); 
	Color = aColor;
	TexCoords = aTexCoords;
	Position = (view * vec4(aPos, 1)).xyz;
}  