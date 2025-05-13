#include "structures.h"

// Definições das variáveis globais
float camX = 90.0f, camY = 10.0f, camZ = 10.0f;
float dirX = -1.0f, dirY = 0.0f, dirZ = -1.0f;
float angleX = 0.0f, angleY = 45.0f;
int centerX, centerY;
float sensitivity = 0.1f;
float smoothFactor = 0.5f;
bool shiftPressed = false;
bool inside = false;

vector<Vertex> verticesInside;
vector<Face> facesInside;
map<string, Material> materialsInside;

vector<Vertex> verticesOutside;
vector<Face> facesOutside;
map<string, Material> materialsOutside;

Material defaultMaterial;
Material* currentMaterial = &defaultMaterial;

GLuint displayListInside = 0;
GLuint displayListOutside = 0;
bool displayListGenerated = false;

CollisionMesh collisionMeshInside;
CollisionMesh collisionMeshOutside;

// Implementação do construtor de Material
Material::Material() : shininess(0.0f) {
    for(int i = 0; i < 3; i++) {
        ambient[i] = 0.2f;
        diffuse[i] = 0.8f;
        specular[i] = 1.0f;
    }
}