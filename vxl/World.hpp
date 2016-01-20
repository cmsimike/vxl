#pragma once
#include "StaticRenderer.hpp"
#include "Shader.hpp"

class World {
public:
	World();
	~World();
	void Draw(Shader* shader);
	void Update();
private:
	int m_chunkSize;
	StaticRenderer* m_renderer = nullptr;
	std::mt19937 m_generator;
	std::uniform_int_distribution<> m_typeRandom;
	std::uniform_int_distribution<> m_shadeRandom;

	void _Create();
};