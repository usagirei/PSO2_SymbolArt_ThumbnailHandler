#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec2 aDrawMode;

out vec4 Color;
out vec2 TexCoords;
out vec3 Position;
flat out ivec2 DrawMode;

uniform mat4 m_View;
uniform mat4 m_Proj;

void main()
{
    gl_Position = m_Proj * m_View * vec4(aPos.x, aPos.y, aPos.z, 1.0); 
	Color = aColor;
	TexCoords = aTexCoords;
	Position = (m_View * vec4(aPos, 1)).xyz;
	DrawMode = aDrawMode;
}  