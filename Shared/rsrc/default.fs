#version 330 core
out vec4 FragColor;

in vec3 Position;
in vec4 Color;  
in vec2 TexCoords;

uniform sampler2D tex;

void main()
{ 
    vec4 t = texture(tex, TexCoords);
    FragColor = t * Color;
}