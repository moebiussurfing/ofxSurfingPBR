OF_GLSL_SHADER_HEADER

uniform sampler2D displacement;

in vec2 texCoordVarying;
in vec3 normalVarying;
out vec4 outputColor;


void main()
{
    outputColor = texture(displacement, texCoordVarying);
}
