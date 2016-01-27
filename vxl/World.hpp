#pragma once
#include "StaticRenderer.hpp"
#include "Shader.hpp"
#include "Chunk.hpp"

class World {
public:
	World();
	~World();
	void Draw(Shader* shader, Shader* water);
	void Update();
private:
	int m_chunkSize;
	int m_seed;
	float m_time;
	float m_wave;
	std::mt19937 m_generator;
	std::uniform_int_distribution<> m_shadeRandom;
	std::vector<Chunk*> m_chunks;

	void _Create();
};