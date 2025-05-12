#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Variáveis para a posição da esfera
float sphereX = 0.0f;
float sphereY = 0.0f;
float sphereZ = -5.0f;

// Variáveis para a direção do movimento
float dirX = 0.01f;
float dirY = 0.01f;
float dirZ = 0.01f;

// Tamanho da esfera
float sphereRadius = 0.5f;

// Função para inicializar parâmetros de rendering
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fundo preto
    glEnable(GL_DEPTH_TEST); // Habilita teste de profundidade
    
    // Configura a iluminação
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat lightPos[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    GLfloat ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
    GLfloat diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    
    // Material da esfera
    GLfloat matAmbient[4] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat matDiffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat matSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShininess = 50.0f;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, matShininess);
    
    // Inicializa o gerador de números aleatórios
    srand(static_cast<unsigned>(time(nullptr)));
}

// Função para desenhar a cena
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Posiciona a câmera
    gluLookAt(0.0, 0.0, 0.0,  // Posição da câmera
              0.0, 0.0, -1.0, // Para onde a câmera aponta
              0.0, 1.0, 0.0); // Vetor "para cima"
    
    // Desenha a esfera na posição atual
    glPushMatrix();
    glTranslatef(sphereX, sphereY, sphereZ);
    glutSolidSphere(sphereRadius, 32, 32);
    glPopMatrix();
    
    glutSwapBuffers();
}

// Função chamada quando a janela é redimensionada
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Função para atualizar a posição da esfera
void update(int value) {
    // Atualiza a posição
    sphereX += dirX;
    sphereY += dirY;
    sphereZ += dirZ;
    
    // Verifica os limites e inverte a direção se necessário
    if (sphereX > 3.0f || sphereX < -3.0f) {
        dirX = -dirX * (0.8f + (rand() % 40) / 100.0f); // Muda a direção e velocidade
    }
    if (sphereY > 3.0f || sphereY < -3.0f) {
        dirY = -dirY * (0.8f + (rand() % 40) / 100.0f);
    }
    if (sphereZ > -2.0f || sphereZ < -8.0f) {
        dirZ = -dirZ * (0.8f + (rand() % 40) / 100.0f);
    }
    
    // Ocasionalmente muda a direção aleatoriamente
    if (rand() % 100 < 2) { // 5% de chance a cada frame
        dirX = (rand() % 100 - 50) / 1000.0f;
        dirY = (rand() % 100 - 50) / 1000.0f;
        dirZ = (rand() % 100 - 50) / 1000.0f;
    }
    
    // Agenda a próxima atualização
    glutTimerFunc(16, update, 0); // ~60 FPS
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Esfera Movendo Aleatoriamente em 3D");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, update, 0); // Primeira chamada do timer
    
    glutMainLoop();
    return 0;
}
