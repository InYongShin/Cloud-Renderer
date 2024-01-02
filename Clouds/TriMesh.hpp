#ifndef TriMesh_hpp
#define TriMesh_hpp


#include "GLTools.hpp"
#include "Program.hpp"

#include <vector>


namespace IY {

const int SPHERE_STACK = 63;
const int SPHERE_SLICE = 65;

struct MeshData {
	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> norms;
	std::vector<glm::vec2> tcoords;
	std::vector<glm::vec3> t3coords;
	std::vector<glm::u32vec3> tris;

	void clear() {
		verts.clear();
		norms.clear();
		tcoords.clear();
		tris.clear();
	}

	MeshData createCube() {
		MeshData data;
		data.verts = { {-1,1,1}, {1,1,1}, {1,-1,1}, {-1,-1,1},
					  {1,1,-1}, {1,-1,-1}, {-1,1,-1}, {-1,-1,-1} };
		data.t3coords = { {0,1,0}, {1,1,0}, {1,0,0}, {0,0,0},
						 {1,1,-1}, {1,0,-1}, {0,1,-1}, {0,0,-1} };
		data.tris = { {0,3,2}, {2,1,0}, {1,2,5}, {5,4,1},
					 {4,5,7}, {7,6,4}, {6,7,3}, {3,0,6},
					 {0,1,6}, {1,4,6}, {2,3,7}, {7,5,2 } };
		return data;
	}


	MeshData createQuad() {
		MeshData data;
		data.verts = {{-1,1,0},{-1,-1,0},{1,-1,0},{1,1,0}};
		data.norms = {{0,0,1},{0,0,1},{0,0,1},{0,0,1}};
		data.tcoords= {{0,0},{0,1},{1,1},{1,0}};
		data.tris = {{0,1,2},{0,2,3}};
		return data;
	}

	MeshData createPlane() {
		MeshData data;
		data.verts = { {-1,0,1},{1,0,1},{1,0,-1},{-1,0,-1} };
		data.norms = { {0,1,0},{0,1,0},{0,1,0},{0,1,0} };
		data.tcoords = { {0,0},{1,0},{1,1},{0,1} };
		data.tris = { {0,1,2},{0,2,3} };
		return data;
	}

	MeshData createSphere(float r) {
		MeshData data;
		int stacks = SPHERE_STACK;
		int slices = SPHERE_SLICE + 1;
		data.verts.push_back(glm::vec3(0, 1, 0) * r);
		data.norms.push_back(glm::vec3(0, 1, 0));
		data.tcoords.push_back(glm::vec2(0.5, 0));
		for (int y = 1; y < stacks; y++) {
			float phi = pi / 2 - y * pi / (stacks);
			for (int x = 0; x < slices; x++) {
				float theta = x * 2 * pi / (slices - 1);
				data.verts.push_back(glm::vec3(cosf(phi) * sinf(theta), sinf(phi), cosf(phi) * cosf(theta)) * r);
				data.norms.push_back(glm::vec3(cosf(phi) * sinf(theta), sinf(phi), cosf(phi) * cosf(theta)));
				data.tcoords.push_back(glm::vec2(x / (float)(slices - 1), y / (float)stacks));
			}
		}
		data.verts.push_back(glm::vec3(0, -1, 0) * r);
		data.norms.push_back(glm::vec3(0, -1, 0));
		data.tcoords.push_back(glm::vec2(0.5, 1));

		for (int x = 0; x < slices - 1; x++)
			data.tris.push_back(glm::u32vec3(0, x + 1, x + 2));
		for (int y = 0; y < stacks - 2; y++) {
			for (int x = 0; x < slices - 1; x++) {
				data.tris.push_back(glm::uvec3(1 + y * slices + x, 1 + (y + 1) * slices + x, 1 + (y + 1) * slices + x + 1));
				data.tris.push_back(glm::uvec3(1 + y * slices + x, 1 + (y + 1) * slices + x + 1, 1 + y * slices + x + 1));
			}
		}
		int y = stacks - 2;
		int last = 1 + (stacks - 1) * slices;
		for (int x = 0; x < slices - 1; x++)
			data.tris.push_back(glm::u32vec3(1 + y * slices + x, last, 1 + y * slices + x + 1));
		return data;
	}

};



class TriMesh {
public:
	MeshData data;

	GLuint vao=0, vBuf=0, eBuf=0, tBuf=0, nBuf=0;
	GLsizei nTris=0, nVerts=0;

	glm::mat4 modelMat = glm::mat4(1);


	void setData(MeshData&& d) {
		data = std::move(d);
	}

	void setData(const MeshData& d) {
		data = d;
	}

	void clear() {
		if (vao) glDeleteVertexArrays(1, &vao); vao = 0;
		if (vBuf) glDeleteBuffers(1, &vBuf); vBuf = 0;
		if (nBuf) glDeleteBuffers(1, &nBuf); nBuf = 0;
		if (tBuf) glDeleteBuffers(1, &tBuf); tBuf = 0;
		if (eBuf) glDeleteBuffers(1, &eBuf); eBuf = 0;
		data.clear();
		nTris = 0;
		nVerts = 0;
	}

	void createMeshGL() {
		if (vao) glDeleteVertexArrays(1, &vao);
		if (vBuf) glDeleteBuffers(1, &vBuf);
		if (nBuf) glDeleteBuffers(1, &nBuf);
		if (tBuf) glDeleteBuffers(1, &tBuf);
		if (eBuf) glDeleteBuffers(1, &eBuf);
		nTris = GLsizei(data.tris.size());
		nVerts = GLsizei(data.verts.size());

 		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vBuf);
		glBindBuffer(GL_ARRAY_BUFFER, vBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * nVerts, data.verts.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		if (data.norms.size() > 0) {
			glGenBuffers(1, &nBuf);
			glBindBuffer(GL_ARRAY_BUFFER, nBuf);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * nVerts, data.norms.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (data.tcoords.size() > 0) {
			glGenBuffers(1, &tBuf);
			glBindBuffer(GL_ARRAY_BUFFER, tBuf);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * nVerts, data.tcoords.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (data.t3coords.size() > 0) {
			glGenBuffers(1, &tBuf);
			glBindBuffer(GL_ARRAY_BUFFER, tBuf);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * nVerts, data.t3coords.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		glGenBuffers(1, &eBuf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eBuf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::u32vec3) * nTris, data.tris.data(), GL_STATIC_DRAW);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		data.clear();
		glErr("create mesh gl");
	}

	void render(const Program& prog, const glm::mat4& modelMat_=glm::mat4(1)) {
		glBindVertexArray(vao);
		prog.setUniform("modelMat", modelMat_*modelMat);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eBuf);
		glDrawElements(GL_TRIANGLES, nTris*3, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void createQuad() {
		clear();
		setData(data.createQuad());
		createMeshGL();
	}

	void createCube() {
		clear();
		setData(data.createCube());
		createMeshGL();
	}

	void createPlane() {
		clear();
		setData(data.createPlane());
		createMeshGL();
	}

	void createSphere(float r) {
		clear();
		setData(data.createSphere(r));
		createMeshGL();
	}

	void renderQuad(const Program& prog, const glm::mat4& modelMat=glm::mat4(1)) {
		if(nVerts<1) createQuad();
		render(prog, modelMat);
	}

	void renderCube(const Program& prog, const glm::mat4& modelMat = glm::mat4(1)) {
		if (nVerts < 1) createCube();
		render(prog, modelMat);
	}

	void renderPlane(const Program& prog, const glm::mat4& modelMat = glm::mat4(1)) {
		if (nVerts < 1) createPlane();
		render(prog, modelMat);
	}

	void renderSphere(const Program& prog, const float radius = 1.f, const glm::mat4& modelMat = glm::mat4(1)) {
		if (nVerts < 1) createSphere(radius);
		render(prog, modelMat);
	}

	TriMesh()
	: vao(0), vBuf(0), eBuf(0), tBuf(0), nBuf(0), nTris(0), nVerts(0), modelMat(glm::mat4(1)) {}

};


}


#endif /* TriMesh_hpp */
