#version 330 core

in vec3 Position;
in vec4 Color;  
in vec2 TexCoords;
flat in ivec2 DrawMode;

out vec4 FragColor;

uniform sampler2D m_Tex;
uniform mat4 m_TexSelector[4];
uniform vec4 m_TexMixer[4];
uniform float m_SdfOffset;
uniform bool m_SdfEnable;


vec4 cubic(float v){
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) * (1.0/6.0);
}

vec4 textureBicubic(sampler2D sampler, vec2 texCoords){

    vec2 texSize = textureSize(sampler, 0);
    vec2 invTexSize = 1.0 / texSize;

    texCoords = texCoords * texSize - 0.5;

    vec2 fxy = fract(texCoords);
    texCoords -= fxy;

    vec4 xcubic = cubic(fxy.x);
    vec4 ycubic = cubic(fxy.y);

    vec4 c = texCoords.xxyy + vec2 (-0.5, +1.5).xyxy;

    vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    vec4 offset = c + vec4(xcubic.yw, ycubic.yw) / s;

    offset *= invTexSize.xxyy;

    vec4 sample0 = texture(sampler, offset.xz);
    vec4 sample1 = texture(sampler, offset.yz);
    vec4 sample2 = texture(sampler, offset.xw);
    vec4 sample3 = texture(sampler, offset.yw);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(
       mix(sample3, sample2, sx), mix(sample1, sample0, sx)
    , sy);
}

void main()
{ 
    int selector = DrawMode.x;
    int sdfFactor = DrawMode.y;

    mat4 texSel = m_TexSelector[selector];
    vec4 texMix = m_TexMixer[selector];
    vec4 texCol = textureBicubic(m_Tex, TexCoords) * texSel;
    
    vec4 shape = mix(texCol, vec4(1), texMix);

    if(m_SdfEnable) {
        float dist = texCol.w - m_SdfOffset;
        float delta = fwidth(dist) * 0.5;
        float edge = smoothstep(-delta, delta, dist);
    
        vec4 shapeSdf = vec4(1,1,1, edge);
        shape = mix(shape, shapeSdf, sdfFactor);
    }

    FragColor = shape * Color;
}