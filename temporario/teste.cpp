#include <GL/freeglut.h>
#include <string>

void renderText(float x, float y, const std::string &text) {
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text.c_str());
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Define a cor do texto (branco)
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Renderiza o texto na posição (0.1, 0.1)
    renderText(0.1f, 0.1f, "Hello, FreeGLUT!");
    
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Exemplo de Texto com FreeGLUT");
    
    glutDisplayFunc(display);
    
    glutMainLoop();
    return 0;
}