#version 450

in float f_texcoord;
uniform sampler1D mytexture;
out vec4 f_color;

void main(void) {
  f_color = texture(mytexture, f_texcoord).rgba;
  //f_color = 0.5;
  //f_color = vec4(0.0, 1.0, 0.0, 1.0);
  //f_color = vec4(f_texcoord, f_texcoord, f_texcoord, 1.0);
}
