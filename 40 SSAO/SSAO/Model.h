#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <SOIL.h>

#include "Mesh.h"

using namespace std;

class Model {
public:
	Model(GLchar* path);
	void Draw(Shader shader);

	vector<Mesh> meshes;
	vector<Texture> textures_loaded;
private:
	string directory;
	
	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};