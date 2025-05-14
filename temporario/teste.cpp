#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

struct Vertex {
    float x, y, z;
};

struct Face {
    int v1, v2, v3;
};

vector<Vertex> vertices;
vector<Face> faces;
bool modelLoaded = false;

float modelX = 0.0f;
float modelY = 0.0f;
float modelZ = -5.0f;
float dirX = 0.01f;
float dirY = 0.01f;
float dirZ = 0.01f;
float modelScale = 10.0f;

bool loadGhost(const char* path) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << path << endl;
        return false;
    }

    string line;
    vertices.clear();
    faces.clear();

    while(getline(file, line)) {
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
            
            // Remove texturas/normais se existirem
            v1 = v1.substr(0, v1.find('/'));
            v2 = v2.substr(0, v2.find('/'));
            v3 = v3.substr(0, v3.find('/'));
            
            f.v1 = stoi(v1) - 1;
            f.v2 = stoi(v2) - 1;
            f.v3 = stoi(v3) - 1;

            faces.push_back(f);
        }
    }

    file.close();
    cout << "Modelo do fantasma carregado com sucesso!" << endl;
    cout << "Vértices: " << vertices.size() << endl;
    cout << "Faces: " << faces.size() << endl;
    
    return true;
}

void drawModel() {
    if (!modelLoaded || vertices.empty() || faces.empty()) return;

    glBegin(GL_TRIANGLES);
    for (const auto& face : faces) {
        // Verificação adicional de segurança
        if (face.v1 < 0 || face.v1 >= vertices.size() ||
            face.v2 < 0 || face.v2 >= vertices.size() ||
            face.v3 < 0 || face.v3 >= vertices.size()) {
            cerr << "Índice de vértice inválido durante o desenho" << endl;
            continue;
        }

        const Vertex& v1 = vertices[face.v1];
        const Vertex& v2 = vertices[face.v2];
        const Vertex& v3 = vertices[face.v3];
        
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat lightPos[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    GLfloat ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
    GLfloat diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    
    GLfloat matAmbient[4] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat matDiffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat matSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShininess = 50.0f;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, matShininess);
    
    modelLoaded = loadGhost("teste.obj");
    if (!modelLoaded) {
        cerr << "Carregando esfera substituta..." << endl;
    }
    
    srand(static_cast<unsigned>(time(nullptr)));
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
    
    glPushMatrix();
    glTranslatef(modelX, modelY, modelZ);
    glScalef(modelScale, modelScale, modelScale);
    
    if (modelLoaded && !vertices.empty() && !faces.empty()) {
        drawModel();
    } else {
        // Desenha uma esfera como fallback
        glutSolidSphere(0.5, 16, 16);
    }
    
    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    modelX += dirX;
    modelY += dirY;
    modelZ += dirZ;
    
    if (modelX > 3.0f || modelX < -3.0f) dirX = -dirX * (0.8f + (rand() % 40) / 100.0f);
    if (modelY > 3.0f || modelY < -3.0f) dirY = -dirY * (0.8f + (rand() % 40) / 100.0f);
    if (modelZ > -2.0f || modelZ < -8.0f) dirZ = -dirZ * (0.8f + (rand() % 40) / 100.0f);
    
    if (rand() % 100 < 5) {
        dirX = (rand() % 100 - 50) / 1000.0f;
        dirY = (rand() % 100 - 50) / 1000.0f;
        dirZ = (rand() % 100 - 50) / 1000.0f;
    }
    
    glutTimerFunc(16, update, 0);
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Modelo OBJ com Movimento Aleatório");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0);
    
    glutMainLoop();
    return 0;
}