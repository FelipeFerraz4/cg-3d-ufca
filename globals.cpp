#include "structures.h"

// Definições das variáveis globais
float camX = -16.093f, camY = 1.40001f, camZ = -0.982863f;
float dirX = 0.997792f, dirY = -0.0262446f, dirZ = 0.0610081f;
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

Ghost ghost1 = {
    .x = 45.0f, .y = 7.5f, .z = 10.0f,
    .dirX = 0.05f, .dirY = 0.05f, .dirZ = 0.05f,
    .radius = 0.5f,
    .matAmbient = {0.7f, 0.7f, 0.7f, 1.0f},
    .matDiffuse = {0.8f, 0.8f, 0.8f, 1.0f},
    .matSpecular = {1.0f, 1.0f, 1.0f, 1.0f},
    .matShininess = 50.0f
};

Ghost ghost2 = {
    .x = 45.0f, .y = 7.5f, .z = 10.0f,
    .dirX = 0.05f, .dirY = 0.05f, .dirZ = 0.05f,
    .radius = 0.5f,
    .matAmbient = {0.7f, 0.7f, 0.7f, 1.0f},
    .matDiffuse = {0.8f, 0.8f, 0.8f, 1.0f},
    .matSpecular = {1.0f, 1.0f, 1.0f, 1.0f},
    .matShininess = 50.0f
};

Ghost ghost3 = {
    .x = 45.0f, .y = 7.5f, .z = 10.0f,
    .dirX = 0.05f, .dirY = 0.05f, .dirZ = 0.05f,
    .radius = 0.5f,
    .matAmbient = {0.7f, 0.7f, 0.7f, 1.0f},
    .matDiffuse = {0.8f, 0.8f, 0.8f, 1.0f},
    .matSpecular = {1.0f, 1.0f, 1.0f, 1.0f},
    .matShininess = 50.0f
};

vector<Vertex> ghostVertices;
vector<Face> ghostFaces;
float scaleGhost = 10.0f;