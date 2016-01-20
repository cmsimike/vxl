#include "stdafx.hpp"
#include "Shader.hpp"

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
	int vertex = _CompileShader(vertexPath, GL_VERTEX_SHADER);
	int fragment = _CompileShader(fragmentPath, GL_FRAGMENT_SHADER);
	m_program = glCreateProgram();
	glAttachShader(m_program, vertex);
	glAttachShader(m_program, fragment);
	glLinkProgram(m_program);
}

int Shader::_CompileShader(const std::string& path, int shaderType) {
	int shaderCompileStatus;
	char compileLog[1024];
	unsigned int shader = glCreateShader(shaderType);
	std::string& shaderSourceString = _LoadShaderFile(path);
	char const* shaderSource = shaderSourceString.c_str();
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompileStatus);
	if (shaderCompileStatus != GL_TRUE) {
		glGetShaderInfoLog(shader, 1024, NULL, compileLog);
		std::cerr << "Shader of type \"" << ((shaderType == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT") << "\" failed to compile!" << std::endl;
		std::cerr << compileLog << std::endl;
	}
	return shader;
}

void Shader::Bind() {
	glUseProgram(m_program);
}

void Shader::Unbind() {
	glUseProgram(0);
}

void Shader::Uniform1f(const std::string& name, float x) {
	int uniform = _GetUniformFromName(name);
	if (uniform != -1)
		glUniform1f(uniform, x);
}

void Shader::Uniform1i(const std::string& name, int x) {
	int uniform = _GetUniformFromName(name);
	if (uniform != -1)
		glUniform1i(uniform, x);
}


void Shader::Uniform2f(const std::string& name, float x, float y) {
	int uniform = _GetUniformFromName(name);
	if (uniform != -1)
		glUniform2f(uniform, x, y);
}

void Shader::Uniform3f(const std::string& name, float x, float y, float z) {
	int uniform = _GetUniformFromName(name);
	if (uniform != -1)
		glUniform3f(uniform, x, y, z);
}

void Shader::Uniform4f(const std::string& name, float x, float y, float z, float w) {
	int uniform = _GetUniformFromName(name);
	if (uniform != -1)
		glUniform4f(uniform, x, y, z, w);
}

void Shader::UniformMat4(const std::string& name, const glm::mat4& matrix) {
	int uniform = _GetUniformFromName(name);
	if (uniform != -1)
		glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix[0][0]);
}

bool Shader::IsInitialized() {
	return (m_program > -1);
}

int Shader::_GetUniformFromName(const std::string& name) {
	GLint uniformLocation = -1;
	if (m_uniforms.count(name) < 1) {
		uniformLocation = glGetUniformLocation(m_program, name.c_str());
		if (uniformLocation != -1) {
			m_uniforms.insert(std::pair<const std::string&, int>(name, uniformLocation));
		}
	}
	else if (m_uniforms.count(name) >= 1) {
		uniformLocation = m_uniforms[name];
	}
	return uniformLocation;
}

std::string Shader::_LoadShaderFile(const std::string& path) {
	std::string fileString;
	std::ifstream fileStream(path, std::ios::in);
	if (fileStream.is_open()) {
		std::string curLine = "";
		while (getline(fileStream, curLine))
			fileString += "\n" + curLine;
		fileStream.close();
	}
	return fileString;
}

Shader::~Shader() {
	glDeleteProgram(m_program);
}