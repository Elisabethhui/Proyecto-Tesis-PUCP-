#version 430

in vec3 Color;
varying float f_texcoord;
uniform sampler1D mytexture;

void main(void)
{
    gl_FragColor = vec4(Color, 1.0);
	gl_FragColor = texture1D(mytexture, f_texcoord).rgba;
}
