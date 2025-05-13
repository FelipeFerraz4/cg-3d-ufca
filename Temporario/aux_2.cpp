#include <GL/freeglut.h>

// Variável para controlar qual cena está ativa
bool cenaExternaAtiva = true;

// Função para desenhar a cena externa
void desenharCenaExterna() {
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f); // Fundo azul escuro (exemplo)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Desenhe objetos da cena externa aqui (ex: um quadrado)
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);
    glEnd();
}

// Função para desenhar a cena interna
void desenharCenaInterna() {
    glClearColor(0.5f, 0.0f, 0.0f, 1.0f); // Fundo vermelho escuro (exemplo)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Desenhe objetos da cena interna aqui (ex: um triângulo)
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 1.0f, 0.0f); // Verde
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
    glEnd();
}

// Função de callback para desenho
void display() {
    if (cenaExternaAtiva) {
        desenharCenaExterna();
    } else {
        desenharCenaInterna();
    }
    glutSwapBuffers();
}

// Função de callback para teclado
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'l':
        case 'L':
            // Alterna entre as cenas ao pressionar 'L' ou 'l'
            cenaExternaAtiva = !cenaExternaAtiva;
            glutPostRedisplay(); // Força o redesenho da tela
            break;
        case 27: // Tecla ESC para sair
            exit(0);
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Troca de Cenas com Tecla L");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}