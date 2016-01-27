#include "stdafx.hpp"
#include "vxl.hpp"

void glfw_resize_callback(GLFWwindow* window, int width, int height) {
	vxl* game = reinterpret_cast<vxl*>(glfwGetWindowUserPointer(window));
	game->Resize(width, height);
}

time_t vxl::beginTime = time(NULL);
float vxl::delta = 0.0f;

vxl::vxl() :
	m_window(nullptr),
	m_width(640),
	m_height(480),
	m_title("vxl"),
	m_camera(static_cast<float>(m_width), static_cast<float>(m_height)) {
}

vxl::~vxl() {
	delete m_defaultShader;
	delete m_waterShader;
	delete m_world;
	if (m_window)
		glfwDestroyWindow(m_window);
	glfwTerminate();
}

int vxl::Stoat() {
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
	// set the width and height just in case
	m_width = width;
	m_height = height;
	// reset the viewport size to the new framebuffer width and height
	glViewport(0, 0, width, height);
	// resize the camera
	m_camera.Resize(static_cast<float>(width), static_cast<float>(height));
}

int vxl::_CreateWindow() {
	if (!glfwInit()) {
		std::cerr << "GLFW failed to initialize!" << std::endl;
		return -1;
	}
	// make this window use OpenGL 3.2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	// use the core profile so that we can have all of the goodness
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// make sure we can use new features if needed
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// make this a resizable window
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
	// enable some things for correct rendering
	_EnableGL();
	// get the main monitor video mode
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	// set the glfw window to the center of the screen
	glfwSetWindowPos(m_window, (mode->width / 2) - (m_width / 2), (mode->height) / 2 - (m_height / 2));
	// set the glfw cursor to hidden
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return 0;
}

void vxl::_EnableGL() {
	glViewport(0, 0, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height));
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void vxl::_Init() {
	m_defaultShader = new Shader(SHADER_DIR + std::string("vertex.glsl"), SHADER_DIR + std::string("fragment.glsl"));
	m_waterShader = new Shader(SHADER_DIR + std::string("water_Vertex.glsl"), SHADER_DIR + std::string("water_Fragment.glsl"));
	m_world = new World();
}

void vxl::_Render() {
	m_world->Draw(m_defaultShader, m_waterShader);
}

void vxl::_Update() {
	// calculate delta time
	m_currentTime = glfwGetTime();
	delta = m_currentTime - m_lastTime;
	m_lastTime = m_currentTime;
	// get the current mouse position
	double mouseX;
	double mouseY;
	glfwGetCursorPos(m_window, &mouseX, &mouseY);
	// rotate the camera by the mouse speed
	m_camera.Rotate(0.005f * static_cast<float>(((m_width / 2.0f) - mouseX)), 0.005f * static_cast<float>(((m_height / 2.0f) - mouseY)), 0.0f);
	// move the camera forward
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
		m_camera.Translate((m_camera.GetDirection() * 10.0f));
	}
	// move the camera backward
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
		m_camera.Translate(-(m_camera.GetDirection() * 10.0f));
	}
	// move the camera left
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
		m_camera.Translate(-(m_camera.GetRight() * 10.0f));
	}
	// move the camera right
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
		m_camera.Translate((m_camera.GetRight() * 10.0f));
	}
	m_defaultShader->Bind();
	m_camera.Update(m_defaultShader);
	m_defaultShader->Unbind();
	m_waterShader->Bind();
	m_camera.Update(m_waterShader);
	m_waterShader->Unbind();
	m_world->Update();
	// set the cursor to the middle of the screen
	glfwSetCursorPos(m_window, static_cast<double>(m_width) / 2.0, static_cast<double>(m_height) / 2.0);
}