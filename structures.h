#pragma once

#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

// Declarações externas
extern float camX, camY, camZ;
extern float dirX, dirY, dirZ;
extern float angleX, angleY;
extern int centerX, centerY;
extern float sensitivity;
extern float smoothFactor;
extern bool shiftPressed;
extern bool inside;

// Constantes
const float PLAYER_RADIUS = 0.3f;
const float PLAYER_HEIGHT = 1.0f;
const float EYE_HEIGHT = 0.8f;

// Estruturas
struct Vertex {
    float x, y, z;
};

struct Material {
    string name;
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float shininess;
    
    Material();
};

struct Face {
    int v1, v2, v3;
    Material* material;
};

struct CollisionMesh {
    vector<Vertex> vertices;
    vector<Face> faces;
};

// Declarações de variáveis globais
extern vector<Vertex> verticesInside;
extern vector<Face> facesInside;
extern map<string, Material> materialsInside;
extern vector<Vertex> verticesOutside;
extern vector<Face> facesOutside;
extern map<string, Material> materialsOutside;
extern Material defaultMaterial;
extern Material* currentMaterial;
extern GLuint displayListInside;
extern GLuint displayListOutside;
extern bool displayListGenerated;
extern CollisionMesh collisionMeshInside;
extern CollisionMesh collisionMeshOutside;

// Declarações de funções
void normalizeHorizontal(float& x, float& z);
void updateCamera();
float clamp(float val, float minVal, float maxVal);
float pointToTriangleDistance(float px, float py, float pz, const Vertex& v0, const Vertex& v1, const Vertex& v2);
bool checkCollision(float newX, float newY, float newZ);
bool checkCollisionWithResponse(float& newX, float& newY, float& newZ, float oldX, float oldY, float oldZ);
void passiveMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);
void specialKeyboardUp(int key, int x, int y);
void update(int value);
bool loadMTL(const char* path, bool isInside);
bool loadOBJ(const char* path, bool isInside);
void generateDisplayLists();
void init();
void updateLightPosition();
void display();
void reshape(int width, int height);
void cleanup();