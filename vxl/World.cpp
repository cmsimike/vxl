#include "stdafx.hpp"
#include "World.hpp"
#include <chrono>

World::World() :
	m_chunkSize(16),
	m_generator(std::random_device()()),
	m_typeRandom(0, VOXEL_TYPE_AMOUNT - 1),
	m_shadeRandom(0, MAX_SHADE - 1) {
	auto start = std::chrono::system_clock::now();
	m_renderer = new StaticRenderer();
	std::thread creationThread(&World::_Create, this);
	creationThread.join();
	m_renderer->Init();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << "ms" << std::endl;
}

World::~World() {
	delete m_renderer;
}

void World::Draw(Shader* shader) {
	m_renderer->Draw(shader);
}

void World::Update() {
	m_renderer->Update();
	m_renderer->Rotate(0.0f, 0.01f, 0.0f);
}

void World::_Create() {
	int maxX = m_chunkSize;
	int maxZ = m_chunkSize;
	for (int z = -m_chunkSize; z < m_chunkSize; z++) {
		maxX = m_chunkSize;
		maxZ = m_chunkSize;
		for (int y = -m_chunkSize; y < m_chunkSize; y++) {
			for (int x = -m_chunkSize; x < m_chunkSize; x++) {
				if (x < maxX && z < maxZ) {
					VoxelPosition pos;
					Voxel voxel;
					pos.x = x;
					pos.y = y;
					pos.z = z;
					// get a random shade for this voxel
					voxel.shade = m_shadeRandom(m_generator);
					// get a random voxel type
					voxel.type = get_random_type(m_typeRandom(m_generator));
					m_renderer->AddVoxel(pos, voxel);
				}
			}
			maxX--;
			maxZ--;
		}
	}
}