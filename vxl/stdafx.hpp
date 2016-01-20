#pragma once
// if we're in debug mode, point to the solution dir
// if not, point to the dir in the game folder
#ifdef _DEBUG
#define SHADER_DIR "../vxl/shader/"
#else
#define SHADER_DIR "shader/"
#endif
// include glew so that we can use new opengl stuff
#include <GL/glew.h>
// define that GLFW will be linked dynamically
#define GLFW_DLL
// include glfw3
#include <GLFW/glfw3.h>
// include glm
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
// include std libraries
#include <map>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <thread>