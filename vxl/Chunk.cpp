#include "stdafx.hpp"
#include "Chunk.hpp"

float noise_fractal_brownian_motion(Perlin noise, int octaves, float x, float y, float z) {
	const float lacunarity = 1.9f;
	const float gain = 0.35f;
	float sum = 0.0f;
	float amplitude = 1.0f;
	int i;
	for (i = 0; i < octaves; i++) {
		sum += amplitude * noise.noise(x, y, z);
		amplitude *= gain;
		x *= lacunarity;
		y *= lacunarity;
		z *= lacunarity;
	}
	return sum;
}

Chunk::Chunk(int x, int y, int z, int width, int height, int depth, int seed) :
	m_chunkX(x),
	m_chunkY(y),
	m_chunkZ(z),
	m_chunkWidth(width),
	m_chunkHeight(height),
	m_chunkDepth(depth),
	m_opaqueRenderer(),
	m_transparentRenderer(),
	m_noise(seed) {
}

void Chunk::Generate(std::mt19937 generator, std::uniform_int_distribution<> shadeRandom) {
	float horizontalFrequency = 1.0f / 24.0f;
	for (int z = m_chunkZ; z < m_chunkZ + m_chunkDepth; z++) {
		for (int x = m_chunkX; x < m_chunkX + m_chunkWidth; x++) {
			// the last type generated
			VoxelType lastType;
			// the height used for this part of the chunk
			int height = static_cast<int>(noise_fractal_brownian_motion(m_noise, 4, x * horizontalFrequency, 0.0f, z * horizontalFrequency) * (m_chunkHeight * 2.0f));
			int maxY = m_chunkY + height;
			if (maxY <= 0) {
				// create three water blocks for water depth
				// shade doesn't matter for water, so just set it to zero
				m_transparentRenderer.AddVoxel(VoxelPosition(x, -1, z), Voxel(VoxelType::WATER, 0));
				m_transparentRenderer.AddVoxel(VoxelPosition(x, -2, z), Voxel(VoxelType::WATER, 0));
				m_transparentRenderer.AddVoxel(VoxelPosition(x, -3, z), Voxel(VoxelType::WATER, 0));
				continue;
			}
			for (int y = m_chunkY; y < maxY; y++) {
				float verticalFrequency = 1.0f / (static_cast<float>(height) / 2.0f);
				float density = noise_fractal_brownian_motion(m_noise, 4, x * horizontalFrequency, y * verticalFrequency, z * horizontalFrequency) + height * m_chunkHeight;
				if (density >= 0.0f) {
					VoxelType type = ((y == maxY - 1 && lastType == VoxelType::DIRT) ? VoxelType::GRASS : _GetTypeFromY(y));
					// check if we should replace a dirt type with grass
					if (y == maxY - 1 && type == VoxelType::DIRT) type = VoxelType::GRASS;
					m_opaqueRenderer.AddVoxel(VoxelPosition(x, y, z), Voxel(type, shadeRandom(generator)));
					// if the y is less than or equal to zero, add three blocks of the same type
					if (y <= 0) {
						m_opaqueRenderer.AddVoxel(VoxelPosition(x, y - 1, z), Voxel(type, shadeRandom(generator)));
						m_opaqueRenderer.AddVoxel(VoxelPosition(x, y - 2, z), Voxel(type, shadeRandom(generator)));
						m_opaqueRenderer.AddVoxel(VoxelPosition(x, y - 3, z), Voxel(type, shadeRandom(generator)));
					}
					lastType = _GetTypeFromY(y);
				}
			}
			lastType = VoxelType::SAND;
		}
	}
}

void Chunk::InitRenderers() {
	m_opaqueRenderer.Init();
	m_transparentRenderer.Init();
}

void Chunk::DrawOpaque(Shader* shader) {
	m_opaqueRenderer.Draw(shader);
}

void Chunk::DrawTransparent(Shader* shader) {
	m_transparentRenderer.Draw(shader);
}

void Chunk::Update() {
	m_opaqueRenderer.Update();
	m_transparentRenderer.Update();
}

VoxelType Chunk::_GetTypeFromY(int y) {
	// if the y is lower than or equal to zero
	// set the type to water
	if (y < 0) return VoxelType::WATER;
	if (y >= 0 && y <= 2) return VoxelType::SAND;
	if (y > 2 && y <= 4) return VoxelType::DIRT;
	if (y > 4 && y == 5) return VoxelType::GRASS;
	if (y > 5) return VoxelType::STONE;
	return VoxelType::GRASS;
}