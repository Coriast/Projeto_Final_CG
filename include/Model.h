#pragma once

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <Mesh.h>
#include <PShader.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model {
public:
	vector<Texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	Model(string const &path, bool gamma = false) : gammaCorrection(gamma) {
		loadModelNew(path);
	}

	Model() = default;
	
	void Draw(PShader& shader) {
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

private:

	void loadModelNew(string path) {
		directory = path.substr(0, path.find_last_of('/'));

		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		vector<glm::vec3> mesh_Positions;
		vector<glm::vec3> mesh_Normals;
		vector<glm::vec2> mesh_TexCoords;

		string mtl_filename;
		fstream obj_file(path);

		while(!obj_file.eof()) {
			string line, type;
			getline(obj_file, line);
			istringstream line_stream(line);
			
			line_stream >> type; // Lê o primeiro conjunto de caracterers separados por espaço

			if (type == "mtllib") { // Nome do MTL para pegar as texturas de Diffusa e Specular
				line_stream >> mtl_filename;
			}
			if (type == "v") { // Coordenadas de Vértice
				float x, y, z;
				line_stream >> x;
				line_stream >> y;
				line_stream >> z;
				mesh_Positions.push_back(glm::vec3(x, y, z));
			}
			if (type == "vn") { // Coordenadas de Normal
				float x, y, z;
				line_stream >> x;
				line_stream >> y;
				line_stream >> z;
				mesh_Normals.push_back(glm::vec3(x, y, z));
			}
			if (type == "vt") { // Coordenadas de Textura
				float x, y;
				line_stream >> x;
				line_stream >> y;
				mesh_TexCoords.push_back(glm::vec2(x, y));
			}
			if (type == "usemtl") {
				//break;
				while (!obj_file.eof()) {
					getline(obj_file, line);
					line_stream = istringstream(line);
					line_stream >> type;
					string face_str;

					if (type == "f" && !line.empty()) {
						unsigned int vertex_Indices[3], texture_Indices[3], normal_Indices[3];
						for (int i = 0; i < 3; i++) {
							line_stream >> face_str;
							face_str.replace(face_str.find_first_of('/'), 1, " ");
							face_str.replace(face_str.find_first_of('/'), 1, " ");
							istringstream face(face_str);
							face >> vertex_Indices[i];	vertex_Indices[i] -= 1;
							face >> texture_Indices[i];	texture_Indices[i] -= 1;
							face >> normal_Indices[i];	normal_Indices[i] -= 1;

							//indices.push_back(vertex_Indices[i]);
						}

						Vertex buffer_vertice;
						for(int i = 0; i < 3; i++) {
							buffer_vertice.Position = mesh_Positions[vertex_Indices[i]];
							buffer_vertice.Normal = mesh_Normals[normal_Indices[i]];
							buffer_vertice.TexCoords = mesh_TexCoords[texture_Indices[i]];
							vertices.push_back(buffer_vertice);
						}
					}
					if (type == "g" || obj_file.eof()) {
						string line_path;
						fstream mtl_file(path.substr(0, path.find_last_of('/') + 1) + mtl_filename);
						int counter_texture = 0;
						Texture diffuse_texture;
						Texture specular_texture;
						while (!mtl_file.eof()) {
							getline(mtl_file, line);
							line_stream = istringstream(line);
							line_stream >> type;
							line_stream >> line_path;
							if (type == "map_Kd" && !line.empty()) {
								diffuse_texture.id = TextureFromFile(line_path.c_str(), this->directory);
								diffuse_texture.type = "material.diffuse";
								diffuse_texture.path = line_path.c_str();
							}
							if (type == "map_Ks" && !line.empty()) {
								specular_texture.id = TextureFromFile(line_path.c_str(), this->directory);
								specular_texture.type = "material.specular";
								specular_texture.path = line_path.c_str();
								counter_texture++;
							}

							if (textures.size() != 0) {
								if (counter_texture == 2) {
									textures.clear();
									textures.push_back(diffuse_texture);
									textures.push_back(specular_texture);
									break;
								}
							} else if (counter_texture == 1){
								textures.push_back(diffuse_texture);
								textures.push_back(specular_texture);
								break;
							}
						}
						for (int i = 0; i < vertices.size(); i++)
						{
							indices.push_back(i);
						}
						meshes.push_back(Mesh(vertices, indices, textures));
						vertices.clear();
						indices.clear();
						break;
					}
				}
			}
		}
	}

};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma) {
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}