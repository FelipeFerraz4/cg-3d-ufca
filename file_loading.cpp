#include "structures.h"

// Função para carregar MTL
bool loadMTL(const char* path, bool isInside) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo MTL: " << path << endl;
        return false;
    }

    string line;
    Material* currentMat = nullptr;
    
    while (getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "newmtl") {
            string name;
            iss >> name;

            if(isInside) {
                materialsInside[name] = Material();
                currentMat = &materialsInside[name];
            } else {
                materialsOutside[name] = Material();
                currentMat = &materialsOutside[name];
            }
            currentMat -> name = name;
        }
        else if (type == "Ka" && currentMat) {
            iss >> currentMat -> ambient[0] >> currentMat -> ambient[1] >> currentMat -> ambient[2];
        }
        else if (type == "Kd" && currentMat) {
            iss >> currentMat -> diffuse[0] >> currentMat -> diffuse[1] >> currentMat -> diffuse[2];
        }
        else if (type == "Ks" && currentMat) {
            iss >> currentMat -> specular[0] >> currentMat -> specular[1] >> currentMat -> specular[2];
        }
        else if (type == "Ns" && currentMat) {
            iss >> currentMat -> shininess;
        }
    }

    file.close();
    cout << "Material " << (isInside? "inside": "outside") << " carregado com sucesso! Materiais: " << (isInside? materialsInside.size(): materialsOutside.size()) << endl;
    return true;
}

// Função para carregar OBJ com otimizações
bool loadOBJ(const char* path, bool isInside) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << path << endl;
        return false;
    }

    string line;
    string mtlPath;
    string nameCurrent = "";
    
    while (getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "v") {
            Vertex v;
            iss >> v.x >> v.y >> v.z;

            if(isInside)
                verticesInside.push_back(v);
            else
                verticesOutside.push_back(v);
        }
        else if (type == "f") {
            Face f;
            string v1, v2, v3;
            iss >> v1 >> v2 >> v3;
            
            sscanf(v1.c_str(), "%d", &f.v1);
            sscanf(v2.c_str(), "%d", &f.v2);
            sscanf(v3.c_str(), "%d", &f.v3);
            
            f.v1--; f.v2--; f.v3--;
            f.material = currentMaterial;
            f.name = nameCurrent;

            if(isInside)
                facesInside.push_back(f);
            else
                facesOutside.push_back(f);
        }
        else if (type == "mtllib") {
            iss >> mtlPath;
            size_t lastSlash = string(path).find_last_of("/\\");
            if (lastSlash != string::npos) {
                mtlPath = string(path).substr(0, lastSlash+1) + mtlPath;
            }
            if (!loadMTL(mtlPath.c_str(), isInside)) {
                cerr << "Aviso: Não foi possível carregar o arquivo MTL: " << mtlPath << endl;
            }
        }
        else if (type == "usemtl") {
            string matName;
            iss >> matName;
            nameCurrent = matName;
            if(isInside) {
                if (materialsInside.find(matName) != materialsInside.end()) {
                    currentMaterial = &materialsInside[matName];
                } else {
                    cerr << "Aviso: Material inside '" << matName << "' não encontrado" << endl;
                    currentMaterial = nullptr;
                }
            } else {
                if (materialsOutside.find(matName) != materialsOutside.end()) {
                    currentMaterial = &materialsOutside[matName];
                } else {
                    cerr << "Aviso: Material outside '" << matName << "' não encontrado" << endl;
                    currentMaterial = nullptr;
                }
            }
        }
    }

    file.close();
    
    // Cria malha simplificada para colisão (1/10 das faces)
    if(isInside) {
        collisionMeshInside.vertices = verticesInside;
        for(size_t i = 0; i < facesInside.size(); i += 10) {
            collisionMeshInside.faces.push_back(facesInside[i]);
        }
        
        cout << "Modelo interno carregado com sucesso!" << endl;
        cout << "Vertices: " << verticesInside.size() << endl;
        cout << "Faces: " << facesInside.size() << endl;
        cout << "Faces de colisão: " << collisionMeshInside.faces.size() << endl;
    } else {
        collisionMeshOutside.vertices = verticesOutside;
        for(size_t i = 0; i < facesOutside.size(); i += 10) {
            collisionMeshOutside.faces.push_back(facesOutside[i]);
        }
        
        cout << "Modelo externo carregado com sucesso!" << endl;
        cout << "Vertices: " << verticesOutside.size() << endl;
        cout << "Faces: " << facesOutside.size() << endl;
        cout << "Faces de colisão: " << collisionMeshOutside.faces.size() << endl;
    }
    
    return true;
}

bool loadGhost(const char* path) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << path << endl;
        return false;
    }

    string line;
    ghostVertices.clear();
    ghostFaces.clear();

    while(getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "v") {
            Vertex v;
            iss >> v.x >> v.y >> v.z;
            ghostVertices.push_back(v);
        }
        else if (type == "f") {
            Face f;
            string v1, v2, v3;
            iss >> v1 >> v2 >> v3;
            
            // Remove texturas/normais se existirem
            v1 = v1.substr(0, v1.find('/'));
            v2 = v2.substr(0, v2.find('/'));
            v3 = v3.substr(0, v3.find('/'));
            
            f.v1 = stoi(v1) - 1;
            f.v2 = stoi(v2) - 1;
            f.v3 = stoi(v3) - 1;
            f.material = &defaultMaterial;  // Usa material padrão

            ghostFaces.push_back(f);
        }
    }

    file.close();
    cout << "Modelo do fantasma carregado com sucesso!" << endl;
    cout << "Vértices: " << ghostVertices.size() << endl;
    cout << "Faces: " << ghostFaces.size() << endl;
    
    return true;
}
