#include <GL/glut.h>
#include <SOIL/SOIL.h> // Biblioteca para carregar texturas
#include <string>
#include <vector>

// Definições de estado do jogo
enum GameState { MENU, PLAYING, OPTIONS, EXITING };
GameState currentState = MENU;

// Variáveis para controle do menu
int selectedOption = 0;
std::vector<std::string> menuOptions = {"o", "o", "o"};

// Configurações da janela
int windowWidth = 800;
int windowHeight = 600;

// Variável para a textura de fundo
GLuint backgroundTexture;

// Função para carregar texturas
void loadTextures() {
    // Carrega a imagem de fundo (substitua pelo caminho correto da sua imagem)
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

void drawText(float x, float y, const std::string &text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void drawMenu() {
    // Configuração da projeção ortogonal para desenho 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, windowHeight, 0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Limpa o buffer de cores
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Desenha o fundo primeiro
    drawBackground();


    
    // Habilita blending para texto com transparência
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Desenha o título do jogo
    glColor4f(1.0f, 1.0f, 1.0f, 0.9f); // Branco com transparência
    
    // Desenha as opções do menu
    for (int i = 0; i < menuOptions.size(); ++i) {
        if (i == selectedOption) {
            glColor4f(1.0f, 0.0f, 0.0f, 0.9f); // Vermelho para opção selecionada
        } else {
            glColor4f(1.0f, 1.0f, 1.0f, 0.7f); // Branco para outras opções
        }
        
        drawText(windowWidth/2 - 100, 285 + i * 90, menuOptions[i]);
    }
    
    glDisable(GL_BLEND);
    
    // Troca os buffers
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC
            currentState = EXITING;
            break;
        case 13: // Enter
            if (currentState == MENU) {
                switch (selectedOption) {
                    case 0: // Iniciar Jogo
                        currentState = PLAYING;
                        break;
                    case 1: // Opções
                        currentState = OPTIONS;
                        break;
                    case 2: // Sair
                        currentState = EXITING;
                        break;
                }
            }
            break;
    }
}

void specialKeys(int key, int x, int y) {
    if (currentState == MENU) {
        switch (key) {
            case GLUT_KEY_UP:
                selectedOption = (selectedOption - 1 + menuOptions.size()) % menuOptions.size();
                break;
            case GLUT_KEY_DOWN:
                selectedOption = (selectedOption + 1) % menuOptions.size();
                break;
        }
    }
    glutPostRedisplay();
}

void display() {
    switch (currentState) {
        case MENU:
            drawMenu();
            break;
        case PLAYING:
            // Lógica do jogo aqui
            break;
        case OPTIONS:
            // Tela de opções aqui
            break;
        case EXITING:
            exit(0);
            break;
    }
}

int main(int argc, char** argv) {
    // Inicializa o GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Meu Jogo");
    
    // Carrega as texturas
    loadTextures();
    
    // Registra callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    
    // Cor de fundo (será coberta pela textura)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Inicia o loop principal
    glutMainLoop();
    
    return 0;
}