#version 330 core
// vertex coordinates
layout(location=0) in vec3 vertex;
layout(location=1) in vec4 color;
// projection matrix
uniform mat4 prj;
// view matrix
uniform mat4 view;
// model matrix
uniform mat4 mdl;
// the color for the fragment
out vec4 fragCol;

void main() {
	gl_Position = (prj * view * mdl) * vec4(vertex, 1.0);
	fragCol = color;
}