#version 450

in vec3 coord3d;
in float texcoord;
out float f_texcoord;
uniform mat4 mvp;

void main(void) {
  gl_Position = mvp * vec4(coord3d, 1.0);
  f_texcoord = texcoord;
}
