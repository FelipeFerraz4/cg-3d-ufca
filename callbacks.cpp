#include "structures.h"

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
        case 'l':
        case 'L':
            inside = !inside;
            cout << "Cena alterada para: " << (inside ? "Interna" : "Externa") << endl;
            break;
    }
    
    checkCollisionWithResponse(newX, newY, newZ, oldX, oldY, oldZ);
    
    camX = newX;
    camY = newY;
    camZ = newZ;
    
    if(aliveGhosts > 0) {
        if(camX>= -4.75 && camX <= -4.35 && camY >= 1.3 && camY <= 1.5 && camZ >= -4.88 && camZ <= -4.08) {
            inside = true;
            camX = 22.01f;
            camY = 4.7f;
            camZ = 3.03f;
        }
        if(camX>= -4.75 && camX <= -4.35 && camY >= 1.3 && camY <= 1.5 && camZ >= -1.0 && camZ <= 0.6) {
            inside = true;
            camX = 22.01f;
            camY = 4.7f;
            camZ = 5.43f;
        }
        if(camX>= -4.75 && camX <= -4.35 && camY >= 1.3 && camY <= 1.5 && camZ >= 3.89 && camZ <= 4.69) {
            inside = true;
            camX = 22.01f;
            camY = 4.7f;
            camZ = 8.22f;
        }
    }

    cout << camX << " " << camY << " " << camZ << endl;
    
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
    
    // Atualiza a posição da esfera apenas na cena interna
    if (inside) {
        for(int i = 0; i < qtdGhosts; i++)
            updateGhost(ghosts[i]);
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}