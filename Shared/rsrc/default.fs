#version 330 core

in vec3 Position;
in vec4 Color;  
in vec2 TexCoords;
flat in int DrawMode;

out vec4 FragColor;

uniform sampler2D m_Tex;
uniform mat4 m_TexSelector[4];
uniform vec4 m_TexMixer[4];

void main()
{ 
    mat4 texSel = m_TexSelector[DrawMode];
    vec4 texMix = m_TexMixer[DrawMode];
    vec4 texCol = texture(m_Tex, TexCoords) * texSel;
    vec4 shape = mix(texCol, vec4(1), texMix);
    FragColor = shape * Color;
}