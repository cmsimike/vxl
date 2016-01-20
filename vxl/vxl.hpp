#pragma once
#include "PerspectiveCamera.hpp"
#include "Shader.hpp"
#include "World.hpp"

void glfw_resize_callback(GLFWwindow* window, int width, int height);

class vxl {
public:
	vxl();
	~vxl();
	int Run();
	void Resize(int width, int height);
private:
	GLFWwindow* m_window;
	unsigned int m_width;
	unsigned int m_height;
	std::string m_title;
	PerspectiveCamera m_camera;
	Shader* m_defaultShader = nullptr;
	World* m_world = nullptr;

	int _CreateWindow();
	void _EnableGL();
	void _Init();
	void _Render();
	void _Update();
};