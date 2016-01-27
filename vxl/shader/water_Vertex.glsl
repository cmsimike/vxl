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
out vec3 fragVert;
// the time constant used for waves
uniform float time;

void main() {
	vec3 vert = vertex;
	float waveDis = distance((abs(1.0 - vert.x) / 2.0), (abs(1.0 - vert.z) / 2.0));
	vert.y = sin(waveDis * time * 0.95) * 4.0;
	gl_Position = (prj * view * mdl) * vec4(vert, 1.0);
	fragCol = color;
	fragVert = vert;
}