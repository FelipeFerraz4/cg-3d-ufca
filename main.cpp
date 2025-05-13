#include "structures.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Jogo Otimizado");
    glutInitWindowSize(1460, 720);
    glutInitWindowPosition(200, 0);

    if(!loadOBJ("teste.obj", true))
        return 1;
    if(!loadOBJ("objetos 3d/Externo.obj", false))
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
    
    atexit(cleanup);
    
    glutMainLoop();
    return 0;
}