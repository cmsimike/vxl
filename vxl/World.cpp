#include "stdafx.hpp"
#include "World.hpp"
#include "vxl.hpp"

World::World() :
	m_chunkSize(16),
	m_generator(std::random_device()()),
	m_shadeRandom(0, MAX_SHADE - 1) {
	// generate a random seed
	srand(time(NULL));
	m_seed = rand();
	// create the actual world
	_Create();
}

void World::_Create() {
	// create about twenty chunks
	for (int x = -5; x < 5; x++)
		for (int z = -5; z < 5; z++) {
			// place a chunk instance at the coordinates with the size
			// the seed is used for the generator
			Chunk* chunk = new Chunk(x * m_chunkSize, 0, z * m_chunkSize, m_chunkSize, m_seed);
			// the thread for generating the chunk
			// pass in the random shade generator as well
			std::thread genThread(&Chunk::Generate, chunk, m_generator, m_shadeRandom);
			// join the thread back to the main loop
			genThread.join();
			// initialize the renderers for this chunk
			// allows for drawing
			chunk->InitRenderers();
			// add the chunk to the chunk vector
			m_chunks.push_back(chunk);
		}
}

void World::Draw(Shader* shader, Shader* water) {
	// for every chunk in the chunk vector, draw it with the bound shader
	// todo: add culling/on-the-fly generation
	shader->Bind();
	for (size_t i = 0; i < m_chunks.size(); i++)
		m_chunks[i]->DrawOpaque(shader);
	shader->Unbind();
	water->Bind();
	glDepthMask(GL_FALSE);
	m_time += 0.01f * vxl::delta;
	water->Uniform1f("time", m_time);
	for (size_t i = 0; i < m_chunks.size(); i++)
		m_chunks[i]->DrawTransparent(water);
	glDepthMask(GL_TRUE);
	water->Unbind();
}

void World::Update() {
	// update every chunk in the chunk vector
	// todo: add culling/on-the-fly generation
	for (size_t i = 0; i < m_chunks.size(); i++)
		m_chunks[i]->Update();
}

World::~World() {
	// for every chunk in the chunk vector
	// delete the pointer to the chunk
	for (size_t i = 0; i < m_chunks.size(); i++)
		delete m_chunks[i];
	// once each pointer is deleted
	// clear the empty references
	m_chunks.clear();
}