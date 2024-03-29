#pragma once
#include "PerspectiveCamera.hpp"
#include "Shader.hpp"
#include "World.hpp"

void glfw_resize_callback(GLFWwindow* window, int width, int height);

class vxl {
public:
	static time_t beginTime;
	static float delta;

	vxl();
	~vxl();
	// twenty nine different attributes, only seven that you like
	int Stoat();
	void Resize(int width, int height);
private:
	GLFWwindow* m_window;
	unsigned int m_width;
	unsigned int m_height;
	float m_currentTime;
	float m_lastTime;
	std::string m_title;
	PerspectiveCamera m_camera;
	Shader* m_defaultShader = nullptr;
	Shader* m_waterShader = nullptr;
	World* m_world = nullptr;

	int _CreateWindow();
	void _EnableGL();
	void _Init();
	void _Render();
	void _Update();
};