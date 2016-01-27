#pragma once
#include "Shader.hpp"
#define VOXEL_TYPE_AMOUNT 3
#define MAX_SHADE 4
#define SHADE_STEP 15
#define AO_MOD 0.16f

enum class VoxelType : unsigned short {
	GRASS = 0,
	DIRT = 1,
	SAND = 2,
	WATER = 3,
	STONE = 4
};

VoxelType get_random_type(int ind);

struct VoxelPosition {
	int x;
	int y;
	int z;
	VoxelPosition() = default;
	VoxelPosition(int x, int y, int z) :
		x(x),
		y(y),
		z(z) {
	}
	inline bool operator==(const VoxelPosition& right) const {
		return (x == right.x) && (y == right.y) && (z == right.z);
	}
	inline bool operator<(const VoxelPosition& right) const {
		return (z < right.z);
	}
};

// compute a hash for the voxel position struct
namespace std {
	template <>
	struct hash<VoxelPosition>
	{
		inline std::size_t operator()(const VoxelPosition& k) const
		{
			return ((hash<int>()(k.x)
				^ (hash<int>()(k.y) << 1)) >> 1)
				^ (hash<int>()(k.z) << 1);
		}
	};
}

struct Voxel {
	int shade;
	VoxelType type;
	// face order:
	// top, bottom, left, right, front, back
	bool faces[6] = { true, true, true, true, true, true };
	Voxel() = default;
	Voxel(VoxelType type, int shade) {
		this->type = type;
		this->shade = shade;
	}
};

struct Color {
	float r;
	float g;
	float b;
	float a;
	Color() = default;
	Color(float r, float g, float b, float a) :
		r(r),
		g(g),
		b(b),
		a(a) {
	}
	inline Color operator=(const Color &right) const {
		// return a new color with the color of the right
		return Color(right.r, right.g, right.b, right.a);
	}
	inline Color operator-(const Color &right) const {
		// subtract each of the components by the other
		// (note: don't do anything to the alpha, if we want to manipulate it, do it manually)
		return Color(this->r - right.r, this->g - right.g, this->b - right.b, this->a);
	}
	inline Color operator-(const float right) const {
		// subtract all of the components by the rvalue
		// (note: don't do anything to the alpha, if we want to manipulate it, do it manually)
		return Color(this->r - right, this->g - right, this->b - right, this->a);
	}
	inline Color operator*(const float right) const {
		// multiply all of the components by the rvalue
		// (note: don't do anything to the alpha, if we want to manipulate it, do it manually)
		return Color(this->r * right, this->g * right, this->b * right, this->a);
	}
};

Color color_from_rgb(unsigned int r, unsigned int g, unsigned int b);

Color get_voxel_color(VoxelType type, int shade);

class StaticRenderer {
public:
	StaticRenderer();
	~StaticRenderer();
	void AddVoxel(VoxelPosition pos, Voxel voxel);
	void Init();
	void Draw(Shader* shader);
	void Update();
	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);
private:
	unsigned int m_vbo;
	unsigned int m_vao;
	unsigned int m_vertCount;
	float m_voxelSize;
	float m_voxelSpacing;
	bool m_initialized;
	std::unordered_map<VoxelPosition, Voxel> m_voxels;
	std::vector<float> m_elements;
	glm::mat4 m_mdl;
	glm::vec3 m_trans;
	glm::vec3 m_rot;
	glm::vec3 m_scale;
	glm::vec3 m_meshMin;
	glm::vec3 m_meshMax;

	void _AddVertex(float x, float y, float z, Color color);
	void _AllocVoxels();
	void _RemoveDuplicateVoxelFaces();
	void _CalculateMeshMinimumAndMaximum(float x, float y, float z, float x1, float y1, float z1);
};