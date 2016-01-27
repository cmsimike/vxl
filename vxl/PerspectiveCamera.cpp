#include "stdafx.hpp"
#include "PerspectiveCamera.hpp"
#include "Shader.hpp"

PerspectiveCamera::PerspectiveCamera(float width, float height) :
	m_fov(65.0f),
	m_nearZ(0.01f),
	m_farZ(1000000.0f),
	m_projection(glm::perspective(glm::radians(m_fov), width / height, m_nearZ, m_farZ)),
	m_view(1.0f) {
}

void PerspectiveCamera::Translate(float x, float y, float z) {
	m_position += glm::vec3(x, y, z);
}

void PerspectiveCamera::Translate(glm::vec3 pos) {
	m_position += pos;
}

void PerspectiveCamera::Rotate(float x, float y, float z) {
	m_rotation += glm::vec3(x, y, z);
}

void PerspectiveCamera::Update(Shader* shader) {
	// the up vector
	glm::vec3 up = glm::cross(GetRight(), GetDirection());
	m_view = glm::lookAt(m_position, m_position + GetDirection(), up);
	// upload to shader
	shader->UniformMat4("prj", m_projection);
	shader->UniformMat4("view", m_view);
}

void PerspectiveCamera::Resize(float width, float height) {
	m_projection = glm::perspective(glm::radians(m_fov), width / height, m_nearZ, m_farZ);
}

glm::vec3 PerspectiveCamera::GetDirection() {
	return glm::vec3(cos(m_rotation.y) * sin(m_rotation.x), sin(m_rotation.y), cos(m_rotation.y) * cos(m_rotation.x));
}

glm::vec3 PerspectiveCamera::GetRight() {
	return glm::vec3(sin(m_rotation.x - glm::pi<float>() / 2.0f), 0.0f, cos(m_rotation.x - glm::pi<float>() / 2.0f));
}