#include <GL/glut.h>
#include <SOIL/SOIL.h> // Biblioteca para carregar texturas
#include <string>
#include <vector>

// Variáveis para a janela
int windowWidth = 800;
int windowHeight = 600;

// Variável para a textura de fundo
GLuint backgroundTexture;

// Função para carregar texturas
void loadTextures() {
    // Carrega apenas a imagem de fundo
    backgroundTexture = SOIL_load_OGL_texture(
        "imagens/background.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA
    );
    
    if(!backgroundTexture) {
        printf("Erro ao carregar textura: '%s'\n", SOIL_last_result());
    }
    
    // Configura parâmetros da textura
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Função para desenhar o fundo
void drawBackground() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(windowWidth, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(windowWidth, windowHeight);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, windowHeight);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

void display() {
    // Configuração da projeção ortogonal para desenho 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, windowHeight, 0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Limpa o buffer de cores
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Desenha apenas o fundo
    drawBackground();
    
    // Troca os buffers
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    // Inicializa o GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Imagem de Fundo");
    
    // Carrega apenas a textura de fundo
    loadTextures();
    
    // Registra callback de display
    glutDisplayFunc(display);
    
    // Cor de fundo (será coberta pela textura)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Inicia o loop principal
    glutMainLoop();
    
    return 0;
}