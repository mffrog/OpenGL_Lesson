//
//  Mesh.h
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/17.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#ifndef Mesh_h
#define Mesh_h

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace Mesh {
	class Mesh;
	class Buffer;
	typedef std::shared_ptr<Mesh> MeshPtr;          ///���b�V���f�[�^�|�C���^�^
	typedef std::shared_ptr<Buffer> BufferPtr;      ///���b�V���o�b�t�@�f�[�^�|�C���^�^

													/**
													* �}�e���A���\����
													*/
	struct Material {
		GLenum type;        ///�C���f�b�N�X�f�[�^�^
		GLsizei size;       ///�`�悷��C���f�b�N�X��
		GLvoid* offset;     ///�`��J�n�C���f�b�N�X�̃o�C�g�I�t�Z�b�g
		GLint baseVertex;   ///�C���f�b�N0�Ƃ݂Ȃ���钸�_�z����̈ʒu
		glm::vec4 color;    ///�}�e���A���̐F
	};

	/**
	* ���b�V��
	*/
	class Mesh {
		friend class Buffer;
	public:
		const std::string& Name() const { return name; }
		void Draw(const BufferPtr& buffer) const;

	private:
		Mesh() = default;
		Mesh(const std::string& n, size_t begin, size_t end);
		Mesh(const Mesh&) = default;
		~Mesh() = default;
		Mesh& operator=(const Mesh&) = default;

	private:
		std::string name;
		std::vector<std::string> textureList;
		size_t beginMaterial = 0;
		size_t endMaterial = 0;
	};

	/**
	* ���b�V���o�b�t�@
	*/
	class Buffer {
	public:
		static BufferPtr Create(int vboSize, int iboSize);

		bool LoadMeshFromFile(const char* filename);
		const MeshPtr& GetMesh(const char* name) const;
		const Material& GetMaterial(size_t index) const;
		void BindVAO() const;

	private:
		Buffer() = default;
		~Buffer();
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

	private:
		GLuint vbo = 0;         ///���f���̒��_�f�[�^���i�[����VBO
		GLuint ibo = 0;         ///���f���̃C���f�b�N�X�f�[�^���i�[����IBO
		GLuint vao = 0;         ///���f���pVAO
		GLintptr vboEnd = 0;    ///�ǂݍ��ݍςݒ��_�f�[�^�̏I�[
		GLintptr iboEnd = 0;    ///�ǂݍ��ݍς݃C���f�b�N�X�f�[�^�̏I�[
		std::vector<Material> materialList;     ///�}�e���A�����X�g
		std::unordered_map<std::string, MeshPtr> meshList;  ///���b�V�����X�g
	};
}

#endif /* Mesh_h */