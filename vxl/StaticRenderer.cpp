#include "stdafx.hpp"
#include "StaticRenderer.hpp"

VoxelType get_random_type(int ind) {
	if (ind == 0) return VoxelType::GRASS;
	else if (ind == 1) return VoxelType::DIRT;
	else if (ind == 2) return VoxelType::SAND;
	return VoxelType::GRASS;
}

Color color_from_rgb(unsigned int r, unsigned int g, unsigned int b) {
	// clamp each color to 255
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	// return a color with each component divided by 255
	// the division by 255 is so that the number is between 0.0 and 1.0
	return Color(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, 1.0f);
}

Color get_voxel_color(VoxelType type, int shade) {
	// the modifier for the voxel shade
	// divided by 255 to get a number between 0.0 and 1.0
	float shadeMod = static_cast<float>(shade * SHADE_STEP) / 255.0f;
	// if the type is grass, return a grass color subtracted by the shade amount
	if (type == VoxelType::GRASS) return color_from_rgb(41, 214, 55) - shadeMod;
	// if the type is dirt, return a dirt color subtracted by the shade amount
	else if (type == VoxelType::DIRT) return color_from_rgb(120, 63, 17) - shadeMod;
	// if the type is sand, return a sand color subtracted by the shade amount
	else if (type == VoxelType::SAND) return color_from_rgb(193, 154, 107) - shadeMod;
	// if the type is stone, return a stone color subtracted by the shade amount
	else if (type == VoxelType::STONE) return color_from_rgb(135, 129, 127) - shadeMod;
	// if the type is sand, return a water color subtracted by the shade amount
	else if (type == VoxelType::WATER) {
		Color col = color_from_rgb(0, 75, 195);
		col.a = 0.85f;
		return col;
	}
	// if the type is none of those, return white subtracted by the shade amount
	return Color(1.0f, 1.0f, 1.0f, 1.0f) - shadeMod;
}

StaticRenderer::StaticRenderer() :
	m_vbo(0),
	m_vao(0),
	m_vertCount(0),
	m_initialized(false),
	m_elements(std::vector<float>()),
	m_voxelSize(16.0f),
	m_voxelSpacing(2.0f),
	m_mdl(1.0),
	m_trans(0.0f),
	m_rot(0.0f),
	m_scale(1.0f),
	m_meshMin(0.0f),
	m_meshMax(0.0f) {
}

StaticRenderer::~StaticRenderer() {
	if (m_initialized) {
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
	}
	m_elements.clear();
	m_voxels.clear();
}

void StaticRenderer::AddVoxel(VoxelPosition pos, Voxel voxel) {
	if (!m_initialized) m_voxels.insert(std::pair<VoxelPosition, Voxel>(pos, voxel));
}

void StaticRenderer::Init() {
	if (!m_initialized && m_voxels.size() > 0) {
		// run voxel facing in a new thread
		std::thread dupRemove(&StaticRenderer::_RemoveDuplicateVoxelFaces, this);
		dupRemove.join();
		// run allocation of each voxel in a new thread
		std::thread voxelAlloc(&StaticRenderer::_AllocVoxels, this);
		voxelAlloc.join();
		// the stride for the data
		// stride is the byte amount for each element
		// stride is equal to seven because three coordinates per vertex
		// and four coordinates per color
		int stride = 7 * sizeof(GLfloat);
		// the offset for the vertex data
		// this is zero because the vertex data starts immediately
		void* vertOff = reinterpret_cast<void*>(0);
		// the offset for the color data
		// this is three because it starts right after the vertex data
		// multiplied by the byte size of a float
		void* colOff = reinterpret_cast<void*>(3 * sizeof(GLfloat));
		// generate a vbo
		glGenBuffers(1, &m_vbo);
		// generate a vao
		glGenVertexArrays(1, &m_vao);
		// bind the vao
		glBindVertexArray(m_vao);
		// bind the vbo
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		// set the vbo data
		glBufferData(GL_ARRAY_BUFFER, m_elements.size() * sizeof(GLfloat), &m_elements[0], GL_STATIC_DRAW);
		// set the first attribute to the vertex data
		glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, vertOff);
		// set the second attribute to the color data
		glVertexAttribPointer(1, 4, GL_FLOAT, false, stride, colOff);
		// unbind the vbo
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// unbind the vao
		glBindVertexArray(0);
		// clear the elements
		m_elements.clear();
		// set initialized to true
		m_initialized = true;
	}
}

void StaticRenderer::Update() {
	// set the model matrix to identity and then multiply by
	// scale, translation, and rotation
	m_mdl = glm::mat4(1.0f);
	glm::vec3 center(-(m_meshMax.x + m_meshMin.x) / 2.0f, -(m_meshMax.y + m_meshMin.y) / 2.0f, -(m_meshMax.z + m_meshMin.z) / 2.0f);
	// translate the model matrix
	m_mdl = glm::translate(m_mdl, m_trans);
	// translate the model by the average of the min and max coords
	m_mdl = glm::translate(m_mdl, -center);
	// rotate the model
	m_mdl = glm::rotate(m_mdl, glm::radians(m_rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	m_mdl = glm::rotate(m_mdl, glm::radians(m_rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	m_mdl = glm::rotate(m_mdl, glm::radians(m_rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	// translate the model by the average of the min and max coords
	m_mdl = glm::translate(m_mdl, center);
	// scale the model
	m_mdl = glm::scale(m_mdl, m_scale);
}

void StaticRenderer::Translate(float x, float y, float z) {
	m_trans += glm::vec3(x, y, -z);
}

void StaticRenderer::Rotate(float x, float y, float z) {
	m_rot += glm::vec3(x, y, z);
}

void StaticRenderer::Draw(Shader* shader) {
	// initialize the vao and vbo if that isn't already done
	if (!m_initialized) {
		Init();
	}
	else {
		shader->UniformMat4("mdl", m_mdl);
		// bind the vao
		glBindVertexArray(m_vao);
		// bind the vertex data
		glEnableVertexAttribArray(0);
		// bind the color data
		glEnableVertexAttribArray(1);
		// draw the vao
		glDrawArrays(GL_TRIANGLES, 0, m_vertCount);
		// unbind the color data
		glDisableVertexAttribArray(1);
		// unbind the vertex data
		glDisableVertexAttribArray(0);
		// unbind the vao
		glBindVertexArray(0);
	}
}

void StaticRenderer::_AddVertex(float x, float y, float z, Color color) {
	m_elements.push_back(x);
	m_elements.push_back(y);
	m_elements.push_back(z);
	m_elements.push_back(color.r);
	m_elements.push_back(color.g);
	m_elements.push_back(color.b);
	m_elements.push_back(color.a);
	m_vertCount++;
}

void StaticRenderer::_AllocVoxels() {
	for (auto& kv : m_voxels) {
		// the x coordinate of the voxel
		float voxelX = m_voxelSize + (static_cast<float>(kv.first.x) * m_voxelSize * m_voxelSpacing);
		float voxelX1 = -m_voxelSize + (static_cast<float>(kv.first.x) * m_voxelSize * m_voxelSpacing);
		// the y coordinate of the voxel
		float voxelY = m_voxelSize + (static_cast<float>(kv.first.y) * m_voxelSize * m_voxelSpacing);
		float voxelY1 = -m_voxelSize + (static_cast<float>(kv.first.y) * m_voxelSize * m_voxelSpacing);
		// the z coordinate of the voxel
		float voxelZ = m_voxelSize + -(static_cast<float>(kv.first.z) * m_voxelSize * m_voxelSpacing);
		float voxelZ1 = -m_voxelSize + -(static_cast<float>(kv.first.z) * m_voxelSize * m_voxelSpacing);
		// the random color for a voxel
		Color voxelColor = get_voxel_color(kv.second.type, kv.second.shade);
		bool useAO = (kv.second.type != VoxelType::WATER);

		// get the minimum and maximum from the mesh each generation
		_CalculateMeshMinimumAndMaximum(voxelX, voxelY, voxelZ, voxelX1, voxelY1, voxelZ1);

		if (kv.second.faces[4])
		{
			// ambient occlusion from the front and bottom of this voxel
			bool aoFb = useAO && m_voxels.count(VoxelPosition(kv.first.x, kv.first.y - 1, kv.first.z + 1)) > 0;
			// ambient occlusion from the front and top of this voxel
			bool aoFt = useAO && m_voxels.count(VoxelPosition(kv.first.x, kv.first.y + 1, kv.first.z + 1)) > 0;
			// Front Face Triangle One
			_AddVertex(voxelX1, voxelY1, voxelZ1, ((aoFb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY1, voxelZ1, ((aoFb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY, voxelZ1, ((aoFt) ? voxelColor * AO_MOD : voxelColor));
			// Front Face Triangle Two
			_AddVertex(voxelX, voxelY, voxelZ1, ((aoFt) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY, voxelZ1, ((aoFt) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY1, voxelZ1, ((aoFb) ? voxelColor * AO_MOD : voxelColor));
		}
		if (kv.second.faces[5])
		{
			// ambient occlusion from the back and bottom of this voxel
			bool aoBb = useAO && m_voxels.count(VoxelPosition(kv.first.x, kv.first.y - 1, kv.first.z - 1)) > 0;
			// ambient occlusion from the back and top of this voxel
			bool aoBt = useAO && m_voxels.count(VoxelPosition(kv.first.x, kv.first.y + 1, kv.first.z - 1)) > 0;
			// Back Face Triangle One
			_AddVertex(voxelX1, voxelY1, voxelZ, ((aoBb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY1, voxelZ, ((aoBb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY, voxelZ, ((aoBt) ? voxelColor * AO_MOD : voxelColor));
			// Back Face Triangle Two
			_AddVertex(voxelX, voxelY, voxelZ, ((aoBt) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY, voxelZ, ((aoBt) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY1, voxelZ, ((aoBb) ? voxelColor * AO_MOD : voxelColor));
		}
		if (kv.second.faces[2]) {
			// ambient occlusion from the left and bottom of this voxel
			bool aoLb = useAO && m_voxels.count(VoxelPosition(kv.first.x - 1, kv.first.y - 1, kv.first.z)) > 0;
			// ambient occlusion from the left and top of this voxel
			bool aoLt = useAO && m_voxels.count(VoxelPosition(kv.first.x - 1, kv.first.y + 1, kv.first.z)) > 0;
			// Left Face Triangle One
			_AddVertex(voxelX1, voxelY1, voxelZ1, ((aoLb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY1, voxelZ, ((aoLb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY, voxelZ1, ((aoLt) ? voxelColor * AO_MOD : voxelColor));
			// Left Face Triangle Two
			_AddVertex(voxelX1, voxelY1, voxelZ, ((aoLb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY, voxelZ, ((aoLt) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY, voxelZ1, ((aoLt) ? voxelColor * AO_MOD : voxelColor));
		}
		if (kv.second.faces[3]) {
			// ambient occlusion from the right and bottom of this voxel
			bool aoRb = useAO && m_voxels.count(VoxelPosition(kv.first.x + 1, kv.first.y - 1, kv.first.z)) > 0;
			// ambient occlusion from the right and top of this voxel
			bool aoRt = useAO && m_voxels.count(VoxelPosition(kv.first.x + 1, kv.first.y + 1, kv.first.z)) > 0;
			// Right Face Triangle One
			_AddVertex(voxelX, voxelY1, voxelZ1, ((aoRb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY1, voxelZ, ((aoRb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY, voxelZ1, ((aoRt) ? voxelColor * AO_MOD : voxelColor));
			// Right Face Triangle Two
			_AddVertex(voxelX, voxelY1, voxelZ, ((aoRb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY, voxelZ, ((aoRt) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY, voxelZ1, ((aoRt) ? voxelColor * AO_MOD : voxelColor));
		}
		if (kv.second.faces[0]) {
			// ambient occlusion from the back and top of this voxel
			bool aoBt = useAO && m_voxels.count(VoxelPosition(kv.first.x, kv.first.y + 1, kv.first.z - 1)) > 0;
			// ambient occlusion from the top and right of this voxel
			bool aoTr = useAO && m_voxels.count(VoxelPosition(kv.first.x + 1, kv.first.y + 1, kv.first.z)) > 0;
			// ambient occlusion from the top and left of this voxel
			bool aoTl = useAO && m_voxels.count(VoxelPosition(kv.first.x - 1, kv.first.y + 1, kv.first.z)) > 0;
			// ambient occlusion from the top and back of this voxel
			bool aoTf = useAO && m_voxels.count(VoxelPosition(kv.first.x, kv.first.y + 1, kv.first.z + 1)) > 0;
			// ambient occlusion from the top behind and to the left
			bool aoTfl = useAO && m_voxels.count(VoxelPosition(kv.first.x - 1, kv.first.y + 1, kv.first.z + 1)) > 0 && !(aoBt || aoTl || aoTr || aoTf);
			// ambient occlusion from the top behind and to the right
			bool aoTfr = useAO && m_voxels.count(VoxelPosition(kv.first.x + 1, kv.first.y + 1, kv.first.z + 1)) > 0 && !(aoBt || aoTl || aoTr || aoTf);
			// ambient occlusion from the top behind and to the left
			bool aoTbl = useAO && m_voxels.count(VoxelPosition(kv.first.x - 1, kv.first.y + 1, kv.first.z - 1)) > 0 && !(aoBt || aoTl || aoTr || aoTf);
			// ambient occlusion from the top behind and to the right
			bool aoTbr = useAO && m_voxels.count(VoxelPosition(kv.first.x + 1, kv.first.y + 1, kv.first.z - 1)) > 0 && !(aoBt || aoTl || aoTr || aoTf);
			// Top Face Triangle One
			_AddVertex(voxelX, voxelY, voxelZ1, ((aoTr || aoTf) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY, voxelZ, ((aoBt || aoTl || aoTbl) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY, voxelZ1, ((aoTl || aoTf || aoTfl) ? voxelColor * AO_MOD : voxelColor));
			// Top Face Triangle Two
			_AddVertex(voxelX, voxelY, voxelZ, ((aoBt || aoTr || aoTbr) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY, voxelZ, ((aoBt || aoTl || aoTbl) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY, voxelZ1, ((aoTr || aoTf) ? voxelColor * AO_MOD : voxelColor));
		}
		if (kv.second.faces[1]) {
			// ambient occlusion to the right and bottom of this voxel
			bool aoRb = useAO && m_voxels.count(VoxelPosition(kv.first.x + 1, kv.first.y - 1, kv.first.z)) > 0;
			// ambient occlusion to the front and bottom of this voxel
			bool aoFb = useAO && m_voxels.count(VoxelPosition(kv.first.x, kv.first.y - 1, kv.first.z + 1)) > 0;
			// ambient occlusion to the left and bottom of this voxel
			bool aoLb = useAO && m_voxels.count(VoxelPosition(kv.first.x - 1, kv.first.y - 1, kv.first.z)) > 0;
			// ambient occlusion to the back and bottom of this voxel
			bool aoBb = useAO && m_voxels.count(VoxelPosition(kv.first.x, kv.first.y - 1, kv.first.z - 1)) > 0;
			// Bottom Face Triangle One
			_AddVertex(voxelX, voxelY1, voxelZ1, ((aoFb || aoRb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY1, voxelZ, ((aoLb || aoBb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY1, voxelZ1, ((aoFb || aoLb) ? voxelColor * AO_MOD : voxelColor));
			// Bottom Face Triangle Two
			_AddVertex(voxelX, voxelY1, voxelZ, ((aoRb || aoBb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX1, voxelY1, voxelZ, ((aoLb || aoBb) ? voxelColor * AO_MOD : voxelColor));
			_AddVertex(voxelX, voxelY1, voxelZ1, ((aoFb || aoRb) ? voxelColor * AO_MOD : voxelColor));
		}
	}
	// remove all of the voxels
	m_voxels.clear();
}

void StaticRenderer::_RemoveDuplicateVoxelFaces() {
	for (auto kv : m_voxels) {
		VoxelPosition pos = kv.first;
		Voxel vox = kv.second;

		// if there is a voxel above this voxel
		if (m_voxels.count(VoxelPosition(pos.x, pos.y + 1, pos.z)) > 0) m_voxels[pos].faces[0] = false;
		// if there is a voxel below this voxel
		if (m_voxels.count(VoxelPosition(pos.x, pos.y - 1, pos.z)) > 0 || vox.type == VoxelType::WATER) m_voxels[pos].faces[1] = false;
		// if there is a voxel to the left of this voxel
		if (m_voxels.count(VoxelPosition(pos.x - 1, pos.y, pos.z)) > 0 || vox.type == VoxelType::WATER) m_voxels[pos].faces[2] = false;
		// if there is a voxel to the right of this voxel
		if (m_voxels.count(VoxelPosition(pos.x + 1, pos.y, pos.z)) > 0 || vox.type == VoxelType::WATER) m_voxels[pos].faces[3] = false;
		// if there is a voxel in front of this voxel
		if (m_voxels.count(VoxelPosition(pos.x, pos.y, pos.z + 1)) > 0 || vox.type == VoxelType::WATER) m_voxels[pos].faces[4] = false;
		// if there is a voxel behind this voxel 
		if (m_voxels.count(VoxelPosition(pos.x, pos.y, pos.z - 1)) > 0 || vox.type == VoxelType::WATER) m_voxels[pos].faces[5] = false;
	}
}

void StaticRenderer::_CalculateMeshMinimumAndMaximum(float x, float y, float z, float x1, float y1, float z1) {
	// compute the minimum of the two x coordinates
	float minX = glm::min(x, x1);
	// compute the minimum of the two y coordinates
	float minY = glm::min(y, y1);
	// compute the minimum of the two z coordinates
	float minZ = glm::min(z, z1);
	// compute the maximum of the two x coordinates
	float maxX = glm::max(x, x1);
	// compute the maximum of the two y coordinates
	float maxY = glm::max(y, y1);
	// compute the maximum of the two z coordinates
	float maxZ = glm::max(z, z1);
	// check if any of the coordinates are less than the stored min
	if (m_meshMin.x > minX) m_meshMin.x = minX;
	if (m_meshMin.y > minY) m_meshMin.y = minY;
	if (m_meshMin.z > minZ) m_meshMin.z = minZ;
	// check if any of the coordinates are greater than the stored max
	if (m_meshMax.x < maxX) m_meshMax.x = maxX;
	if (m_meshMax.y < maxY) m_meshMax.y = maxY;
	if (m_meshMax.z < maxZ) m_meshMax.z = maxZ;
}