#version 330 core
// the fragment color
out vec4 color;
// the color from the vertex shader
in vec4 fragCol;
in vec3 fragVert;

void main() {
	vec4 col = vec4(fragCol.r + (fragVert.y / 250.0f), fragCol.g + (fragVert.y / 250.0f), fragCol.b + (fragVert.y / 250.0f), fragCol.a);
	color = col;
}