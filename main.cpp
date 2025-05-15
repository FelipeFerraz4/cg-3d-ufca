#include "structures.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1420, 720);
    glutInitWindowPosition(200, 0);
    glutCreateWindow("Jogo");

    if(!loadOBJ("objetos_3d/Interno.obj", true))
        return 1;
    if(!loadOBJ("objetos_3d/Externo.obj", false))
        return 1;
    if(!loadGhost("objetos_3d/Fantasma.obj"))
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