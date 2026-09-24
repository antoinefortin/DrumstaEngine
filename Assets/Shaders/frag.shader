#version 460 core

in vec2 fragUV;

uniform sampler2D uTexture;
out vec4 outColor;

void main()
{
    outColor = texture(uTexture, fragUV * vec2(12.0f));
}
