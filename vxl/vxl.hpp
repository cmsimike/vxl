#pragma once
#include "PerspectiveCamera.hpp"
#include "Shader.hpp"
#include "World.hpp"

class vxl {
public:
	vxl();
	~vxl();
	int Run();
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