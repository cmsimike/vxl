#include "stdafx.hpp"
#include "PerspectiveCamera.hpp"
#include "Shader.hpp"

PerspectiveCamera::PerspectiveCamera(float width, float height) :
	m_fov(65.0f),
	m_nearZ(0.01f),
	m_farZ(1000.0f),
	m_projection(glm::perspective(glm::radians(m_fov), width / height, m_nearZ, m_farZ)),
	m_view(1.0f) {
}

void PerspectiveCamera::Translate(float x, float y, float z) {
	m_view = glm::translate(m_view, glm::vec3(x, y, z));
}

void PerspectiveCamera::Update(Shader* shader) {
	// upload to shader
	shader->UniformMat4("prj", m_projection);
	shader->UniformMat4("view", m_view);
}

void PerspectiveCamera::Resize(float width, float height) {
	m_projection = glm::perspective(glm::radians(m_fov), width / height, m_nearZ, m_farZ);
}