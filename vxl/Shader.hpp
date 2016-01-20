#pragma once

class Shader {
public:
	Shader() = default;
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();
	void Bind();
	void Unbind();
	void Uniform1f(const std::string& name, float x);
	void Uniform1i(const std::string& name, int x);
	void Uniform2f(const std::string& name, float x, float y);
	void Uniform3f(const std::string& name, float x, float y, float z);
	void Uniform4f(const std::string& name, float x, float y, float z, float w);
	void UniformMat4(const std::string& name, const glm::mat4& matrix);
	bool IsInitialized();
private:
	int m_program = -1;
	std::map<std::string, int> m_uniforms;

	int _CompileShader(const std::string& path, int shaderType);
	int _GetUniformFromName(const std::string& name);
	std::string _LoadShaderFile(const std::string& filePath);
};