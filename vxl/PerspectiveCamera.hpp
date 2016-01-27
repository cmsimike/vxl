#pragma once

class Shader;

class PerspectiveCamera {
public:
	PerspectiveCamera() = default;
	PerspectiveCamera(float width, float height);
	void Translate(float x, float y, float z);
	void Translate(glm::vec3 pos);
	void Rotate(float x, float y, float z);
	void Update(Shader* shader);
	void Resize(float width, float height);
	glm::vec3 GetDirection();
	glm::vec3 GetRight();
private:
	float m_fov;
	float m_nearZ;
	float m_farZ;
	glm::mat4 m_projection;
	glm::mat4 m_view;
	glm::vec3 m_position;
	glm::vec3 m_rotation;
};