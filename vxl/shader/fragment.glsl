#version 330 core
// the fragment color
out vec4 color;
// the color from the vertex shader
in vec4 fragCol;

void main() {
	color = fragCol;
}