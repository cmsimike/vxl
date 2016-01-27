#pragma once
#include "StaticRenderer.hpp"
#include "PerlinNoise.hpp"

inline int clamp_int(float clmp) {
	return (clmp < 1) ? 1 : static_cast<int>(clmp);
}

float noise_fractal_brownian_motion(Perlin noise, int octaves, float x, float y, float z);

class Chunk {
public:
	Chunk() : Chunk(0, 0, 0, 16, 16, 16, rand()) {};
	Chunk(int x, int y, int z, int size) : Chunk(x, y, z, size, size, size, rand()) {};
	Chunk(int x, int y, int z, int size, int seed) : Chunk(x, y, z, size, size, size, seed) {};
	Chunk(int x, int y, int z, int width, int height, int depth, int seed);
	void InitRenderers();
	void DrawOpaque(Shader* shader);
	void DrawTransparent(Shader* shader);
	void Update();
	void Generate(std::mt19937 generator, std::uniform_int_distribution<> m_shadeRandom);
	inline int GetX() const { return m_chunkX; }
	inline int GetY() const { return m_chunkY; }
	inline int GetZ() const { return m_chunkZ; }
private:
	Perlin m_noise;
	int m_chunkX;
	int m_chunkY;
	int m_chunkZ;
	int m_chunkWidth;
	int m_chunkHeight;
	int m_chunkDepth;
	StaticRenderer m_opaqueRenderer;
	StaticRenderer m_transparentRenderer;
	
	VoxelType _GetTypeFromY(int y);
};