#pragma once
#define VOXEL_TYPE_AMOUNT 2
#define MAX_SHADE 3
#define SHADE_STEP 15

enum class VoxelType : unsigned char {
	GRASS = 'g',
	DIRT = 'd'
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
	bool operator==(const VoxelPosition& right) const {
		return (x == right.x) && (y == right.y) && (z == right.z);
	}
	bool operator<(const VoxelPosition& right) const {
		return (x < right.x) && (y < right.y) && (z < right.z);
	}
};

// compute a hash for the voxel position struct
namespace std {
	template <>
	struct hash<VoxelPosition>
	{
		std::size_t operator()(const VoxelPosition& k) const
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
	~Voxel() {
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
	Color operator-(const Color &right) const {
		// subtract each of the components by the other
		float nr = this->r - right.r;
		float ng = this->g - right.g;
		float nb = this->b - right.b;
		float na = this->a - right.a;
		return Color(nr, ng, nb, na);
	}
	Color operator-(const float right) const {
		// subtract all of the components by the rvalue
		float nr = this->r - right;
		float ng = this->g - right;
		float nb = this->b - right;
		float na = this->a - right;
		return Color(nr, ng, nb, na);
	}
};

Color color_from_rgb(unsigned int r, unsigned int g, unsigned int b);

Color get_voxel_color(VoxelType type, int shade);

class Shader;

class StaticRenderer {
public:
	StaticRenderer();
	~StaticRenderer();
	void AddVoxel(VoxelPosition pos, Voxel voxel);
	void Init();
	void Draw(Shader* shader);
	void Update();
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
	glm::mat4 m_transMat;
	glm::mat4 m_rotMat;
	glm::mat4 m_scaleMat;

	void _AddVertex(float x, float y, float z, Color color);
	void _AllocVoxels();
	void _RemoveDuplicateVoxelFaces();
};