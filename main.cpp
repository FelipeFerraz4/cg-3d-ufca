#include <GL/freeglut.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <unordered_map>

using namespace std;

// Variáveis da câmera
float camX = 100.0f, camY = 10.0f, camZ = 100.0f;
float dirX = -1.0f, dirY = 0.0f, dirZ = -1.0f;
float angleX = 0.0f, angleY = 45.0f;
int centerX, centerY;
float sensitivity = 0.1f;
float smoothFactor = 0.5f;
bool shiftPressed = false;

// Parâmetros do jogador
const float PLAYER_RADIUS = 0.3f;
const float PLAYER_HEIGHT = 1.0f;
const float EYE_HEIGHT = 0.8f;

// Estruturas de dados
struct Vertex {
    float x, y, z;
};

struct Material {
    string name;
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float shininess;
    
    Material() : shininess(0.0f) {
        for(int i = 0; i < 3; i++) {
            ambient[i] = 0.2f;
            diffuse[i] = 0.8f;
            specular[i] = 1.0f;
        }
    }
};

struct Face {
    int v1, v2, v3;
    Material* material;
};

// Dados do modelo
vector<Vertex> vertices;
vector<Face> faces;
map<string, Material> materials;
Material defaultMaterial;
Material* currentMaterial = &defaultMaterial;

// Otimizações
GLuint displayList = 0;
bool displayListGenerated = false;

// Malha simplificada para colisão
struct CollisionMesh {
    vector<Vertex> vertices;
    vector<Face> faces;
};
CollisionMesh collisionMesh;

// Função para carregar MTL
bool loadMTL(const char* path) {
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
            materials[name] = Material();
            currentMat = &materials[name];
            currentMat->name = name;
        }
        else if (type == "Ka" && currentMat) {
            iss >> currentMat->ambient[0] >> currentMat->ambient[1] >> currentMat->ambient[2];
        }
        else if (type == "Kd" && currentMat) {
            iss >> currentMat->diffuse[0] >> currentMat->diffuse[1] >> currentMat->diffuse[2];
        }
        else if (type == "Ks" && currentMat) {
            iss >> currentMat->specular[0] >> currentMat->specular[1] >> currentMat->specular[2];
        }
        else if (type == "Ns" && currentMat) {
            iss >> currentMat->shininess;
        }
    }

    file.close();
    cout << "Material carregado com sucesso! Materiais: " << materials.size() << endl;
    return true;
}

// Função para carregar OBJ com otimizações
bool loadOBJ(const char* path) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << path << endl;
        return false;
    }

    string line;
    string mtlPath;
    
    while (getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;

        if (type == "v") {
            Vertex v;
            iss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
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
            faces.push_back(f);
        }
        else if (type == "mtllib") {
            iss >> mtlPath;
            size_t lastSlash = string(path).find_last_of("/\\");
            if (lastSlash != string::npos) {
                mtlPath = string(path).substr(0, lastSlash+1) + mtlPath;
            }
            if (!loadMTL(mtlPath.c_str())) {
                cerr << "Aviso: Não foi possível carregar o arquivo MTL: " << mtlPath << endl;
            }
        }
        else if (type == "usemtl") {
            string matName;
            iss >> matName;
            if (materials.find(matName) != materials.end()) {
                currentMaterial = &materials[matName];
            } else {
                cerr << "Aviso: Material '" << matName << "' não encontrado" << endl;
                currentMaterial = nullptr;
            }
        }
    }

    file.close();
    
    // Cria malha simplificada para colisão (1/10 das faces)
    collisionMesh.vertices = vertices;
    for(size_t i = 0; i < faces.size(); i += 10) {
        collisionMesh.faces.push_back(faces[i]);
    }
    
    cout << "Modelo carregado com sucesso!" << endl;
    cout << "Vertices: " << vertices.size() << endl;
    cout << "Faces: " << faces.size() << endl;
    cout << "Faces de colisão: " << collisionMesh.faces.size() << endl;
    
    return true;
}

// Funções de matemática
void normalizeHorizontal(float& x, float& z) {
    float length = sqrt(x * x + z * z);
    if (length > 0) {
        x /= length;
        z /= length;
    }
}

void updateCamera() {
    dirX = sin(angleY * M_PI / 180) * cos(angleX * M_PI / 180);
    dirY = sin(angleX * M_PI / 180);
    dirZ = -cos(angleY * M_PI / 180) * cos(angleX * M_PI / 180);
    glutPostRedisplay();
}

// Funções de colisão otimizadas
float clamp(float val, float minVal, float maxVal) {
    return max(minVal, min(maxVal, val));
}

float pointToTriangleDistance(float px, float py, float pz, const Vertex& v0, const Vertex& v1, const Vertex& v2) {
    float v0x = v1.x - v0.x;
    float v0y = v1.y - v0.y;
    float v0z = v1.z - v0.z;

    float v1x = v2.x - v0.x;
    float v1y = v2.y - v0.y;
    float v1z = v2.z - v0.z;

    float vx = px - v0.x;
    float vy = py - v0.y;
    float vz = pz - v0.z;

    float d00 = v0x*v0x + v0y*v0y + v0z*v0z;
    float d01 = v0x*v1x + v0y*v1y + v0z*v1z;
    float d11 = v1x*v1x + v1y*v1y + v1z*v1z;
    float d20 = vx*v0x + vy*v0y + vz*v0z;
    float d21 = vx*v1x + vy*v1y + vz*v1z;

    float denom = d00 * d11 - d01 * d01;
    float u = (d11 * d20 - d01 * d21) / denom;
    float v = (d00 * d21 - d01 * d20) / denom;

    if (u >= 0 && v >= 0 && (u + v) <= 1.0f) {
        float projX = v0.x + u * v0x + v * v1x;
        float projY = v0.y + u * v0y + v * v1y;
        float projZ = v0.z + u * v0z + v * v1z;
        float dx = px - projX;
        float dy = py - projY;
        float dz = pz - projZ;
        return sqrt(dx*dx + dy*dy + dz*dz);
    }

    auto pointToSegmentDistance = [](float px, float py, float pz, const Vertex& a, const Vertex& b) {
        float abx = b.x - a.x;
        float aby = b.y - a.y;
        float abz = b.z - a.z;

        float apx = px - a.x;
        float apy = py - a.y;
        float apz = pz - a.z;

        float abLenSq = abx*abx + aby*aby + abz*abz;
        float t = (abx*apx + aby*apy + abz*apz) / abLenSq;
        t = clamp(t, 0.0f, 1.0f);

        float cx = a.x + t * abx;
        float cy = a.y + t * aby;
        float cz = a.z + t * abz;

        float dx = px - cx;
        float dy = py - cy;
        float dz = pz - cz;

        return sqrt(dx*dx + dy*dy + dz*dz);
    };

    float d1 = pointToSegmentDistance(px, py, pz, v0, v1);
    float d2 = pointToSegmentDistance(px, py, pz, v1, v2);
    float d3 = pointToSegmentDistance(px, py, pz, v2, v0);

    return min(d1, min(d2, d3));
}

bool checkCollision(float newX, float newY, float newZ) {
    if (newY - EYE_HEIGHT + PLAYER_HEIGHT < 0) {
        return true;
    }
    
    for (const auto& face : collisionMesh.faces) {
        const Vertex& v1 = collisionMesh.vertices[face.v1];
        const Vertex& v2 = collisionMesh.vertices[face.v2];
        const Vertex& v3 = collisionMesh.vertices[face.v3];
        
        float minX = min(v1.x, min(v2.x, v3.x));
        float maxX = max(v1.x, max(v2.x, v3.x));
        float minY = min(v1.y, min(v2.y, v3.y));
        float maxY = max(v1.y, max(v2.y, v3.y));
        float minZ = min(v1.z, min(v2.z, v3.z));
        float maxZ = max(v1.z, max(v2.z, v3.z));
        
        if (newX > minX - PLAYER_RADIUS && newX < maxX + PLAYER_RADIUS &&
            newY > minY - PLAYER_HEIGHT && newY < maxY + PLAYER_HEIGHT &&
            newZ > minZ - PLAYER_RADIUS && newZ < maxZ + PLAYER_RADIUS) {
            
            float dist = pointToTriangleDistance(newX, newY, newZ, v1, v2, v3);
            if (dist < PLAYER_RADIUS) {
                return true;
            }
        }
    }
    return false;
}

bool checkCollisionWithResponse(float& newX, float& newY, float& newZ, float oldX, float oldY, float oldZ) {
    if (!checkCollision(newX, newY, newZ)) {
        return false;
    }
    
    float testX = newX;
    float testZ = oldZ;
    if (!checkCollision(testX, newY, testZ)) {
        newX = testX;
        newZ = testZ;
        return false;
    }
    
    testX = oldX;
    testZ = newZ;
    if (!checkCollision(testX, newY, testZ)) {
        newX = testX;
        newZ = testZ;
        return false;
    }
    
    newX = oldX;
    newY = oldY;
    newZ = oldZ;
    return true;
}

// Função para gerar a display list
void generateDisplayList() {
    if (displayListGenerated) return;
    
    displayList = glGenLists(1);
    glNewList(displayList, GL_COMPILE);
    
    glBegin(GL_TRIANGLES);
    Material* lastMaterial = nullptr;
    
    for (const auto& face : faces) {
        const Vertex& v1 = vertices[face.v1];
        const Vertex& v2 = vertices[face.v2];
        const Vertex& v3 = vertices[face.v3];
        
        if (face.material != lastMaterial) {
            if (face.material) {
                glMaterialfv(GL_FRONT, GL_AMBIENT, face.material->ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, face.material->diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, face.material->specular);
                glMaterialf(GL_FRONT, GL_SHININESS, face.material->shininess);
            } else {
                glMaterialfv(GL_FRONT, GL_AMBIENT, defaultMaterial.ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultMaterial.diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, defaultMaterial.specular);
                glMaterialf(GL_FRONT, GL_SHININESS, defaultMaterial.shininess);
            }
            lastMaterial = face.material;
        }
        
        float nx, ny, nz;
        float ux = v2.x - v1.x, uy = v2.y - v1.y, uz = v2.z - v1.z;
        float vx = v3.x - v1.x, vy = v3.y - v1.y, vz = v3.z - v1.z;
        nx = uy * vz - uz * vy;
        ny = uz * vx - ux * vz;
        nz = ux * vy - uy * vx;
        
        float len = sqrt(nx*nx + ny*ny + nz*nz);
        if (len > 0) { nx /= len; ny /= len; nz /= len; }

        glNormal3f(nx, ny, nz);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
    
    glEndList();
    displayListGenerated = true;
}

// Inicialização do OpenGL
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(60.0f, 1.0f, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
    glutWarpPointer(centerX, centerY);

    glShadeModel(GL_SMOOTH);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightPosition[] = {0.0f, 5.0f, 5.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void updateLightPosition() {
    GLfloat lightPosition[] = {camX, camY + 2.0f, camZ + 2.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

// Callbacks
void passiveMotion(int x, int y) {
    static float targetAngleX = angleX;
    static float targetAngleY = angleY;

    int deltaX = x - centerX;
    int deltaY = y - centerY;

    if (deltaX != 0 || deltaY != 0) {
        targetAngleY += deltaX * sensitivity;
        targetAngleX -= deltaY * sensitivity;
        targetAngleX = fmax(-89.0f, fmin(89.0f, targetAngleX));

        angleX = angleX * (1.0f - smoothFactor) + targetAngleX * smoothFactor;
        angleY = angleY * (1.0f - smoothFactor) + targetAngleY * smoothFactor;

        updateCamera();
        glutWarpPointer(centerX, centerY);
    }
}

void keyboard(unsigned char key, int x, int y) {
    float speed = 0.2f;
    float moveX = dirX, moveZ = dirZ;
    float oldX = camX, oldY = camY, oldZ = camZ;
    float newX = camX, newY = camY, newZ = camZ;
    
    normalizeHorizontal(moveX, moveZ);

    switch (key) {
        case 'w':
            newX += moveX * speed;
            newZ += moveZ * speed;
            break;
        case 's':
            newX -= moveX * speed;
            newZ -= moveZ * speed;
            break;
        case 'a':
            newX += moveZ * speed;
            newZ += -moveX * speed;
            break;
        case 'd':
            newX += -moveZ * speed;
            newZ += moveX * speed;
            break;
        case ' ':
            newY += speed;
            break;
        case 'c':
            newY -= speed;
            break;
        case 27:
            glutLeaveMainLoop();
            break;
    }
    
    checkCollisionWithResponse(newX, newY, newZ, oldX, oldY, oldZ);
    
    camX = newX;
    camY = newY;
    camZ = newZ;
    
    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R) {
        shiftPressed = true;
    }
    glutPostRedisplay();
}

void specialKeyboardUp(int key, int x, int y) {
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R) {
        shiftPressed = false;
    }
}

void update(int value) {
    if (shiftPressed) {
        float oldY = camY;
        float newY = camY - 0.05f;
        
        if (!checkCollision(camX, newY, camZ)) {
            camY = newY;
        } else {
            camY = oldY;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Renderização otimizada
void display() {
    updateLightPosition();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ, camX + dirX, camY + dirY, camZ + dirZ, 0.0f, 1.0f, 0.0f);

    // Gera a display list na primeira renderização
    if (!displayListGenerated) {
        generateDisplayList();
    }
    
    // Usa a display list para renderização rápida
    glCallList(displayList);

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    float ratio = width * 1.0f / height;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Limpeza de recursos
void cleanup() {
    if (displayListGenerated) {
        glDeleteLists(displayList, 1);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Jogo Otimizado");
    glutInitWindowSize(1460, 720);
    glutInitWindowPosition(200, 0);

    if(!loadOBJ("teste.obj"))
        return 1;

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutSpecialUpFunc(specialKeyboardUp);
    glutPassiveMotionFunc(passiveMotion);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutTimerFunc(0, update, 0);
    
    // Registra função de limpeza
    atexit(cleanup);
    
    glutMainLoop();
    return 0;
}