#include "stdafx.hpp"
#include "vxl.hpp"

void glfw_resize_callback(GLFWwindow* window, int width, int height) {
	vxl* game = reinterpret_cast<vxl*>(glfwGetWindowUserPointer(window));
	game->Resize(width, height);
}

vxl::vxl() :
	m_window(nullptr),
	m_width(640),
	m_height(480),
	m_title("vxl"),
	m_camera((float) m_width, (float) m_height) {
}

vxl::~vxl() {
	delete m_defaultShader;
	delete m_world;
	if (m_window)
		glfwDestroyWindow(m_window);
	glfwTerminate();
}

int vxl::Run() {
	int result = _CreateWindow();
	if (result != 0) return result;
	_Init();
	while (!glfwWindowShouldClose(m_window)) {
		// clear both depth and color each frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_Update();
		_Render();
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
	return 0;
}

void vxl::Resize(int width, int height) {
	m_width = width;
	m_height = height;
	glViewport(0, 0, width, height);
	m_camera.Resize(static_cast<float>(width), static_cast<float>(height));
}

int vxl::_CreateWindow() {
	if (!glfwInit()) {
		std::cerr << "GLFW failed to initialize!" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	this->m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
	if (!m_window) {
		std::cerr << "GLFW failed to create a window!" << std::endl;
		return -2;
	}
	// set the framebuffer resize callback
	glfwSetFramebufferSizeCallback(m_window, glfw_resize_callback);
	// set the user pointer to this instance
	// allows for us to call our class methods in the callbacks
	glfwSetWindowUserPointer(m_window, this);
	glfwMakeContextCurrent(m_window);
	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "GLEW failed to initialize!" << std::endl;
		return -3;
	}
	_EnableGL();
	return 0;
}

void vxl::_EnableGL() {
	glViewport(0, 0, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_width));
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void vxl::_Init() {
	m_defaultShader = new Shader(SHADER_DIR + std::string("vertex.glsl"), SHADER_DIR + std::string("fragment.glsl"));
	m_world = new World();
	m_camera.Translate(0.0f, -32.0f, -500.0f);
}

void vxl::_Render() {
	m_defaultShader->Bind();
	m_world->Draw(m_defaultShader);
	m_defaultShader->Unbind();
}

void vxl::_Update() {
	m_defaultShader->Bind();
	m_camera.Update(m_defaultShader);
	m_defaultShader->Unbind();
	m_world->Update();
}