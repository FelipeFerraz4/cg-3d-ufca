#include "structures.h"
#include <cstdlib>
#include <ctime>

void initGhost() {
    // Inicializa o gerador de números aleatórios
    static bool initialized = false;
    if (!initialized) {
        srand(static_cast<unsigned>(time(nullptr)));
        initialized = true;
    }
}

void updateGhost() {
    // Atualiza a posição
    ghost.x += ghost.dirX;
    ghost.y += ghost.dirY;
    ghost.z += ghost.dirZ;
    
    // Limites da cena interna (ajuste conforme seu modelo)
    const float minX = 20.0f, maxX = 80.0f;
    const float minY = 3.0f, maxY = 10.0f; // Evita que a esfera vá abaixo do chão
    const float minZ = -14.0f, maxZ = 25.0f;
    
    // Verifica os limites e inverte a direção se necessário
    if (ghost.x > maxX || ghost.x < minX) {
        ghost.dirX = -ghost.dirX * (0.8f + (rand() % 40) / 100.0f);
    }
    if (ghost.y > maxY || ghost.y < minY) {
        ghost.dirY = -ghost.dirY * (0.8f + (rand() % 40) / 100.0f);
    }
    if (ghost.z > maxZ || ghost.z < minZ) {
        ghost.dirZ = -ghost.dirZ * (0.8f + (rand() % 40) / 100.0f);
    }
    
    // Ocasionalmente muda a direção aleatoriamente
    if (rand() % 100 < 5) { // 5% de chance a cada frame
        ghost.dirX = (rand() % 100 - 50) / 1000.0f;
        ghost.dirY = (rand() % 100 - 50) / 1000.0f;
        ghost.dirZ = (rand() % 100 - 50) / 1000.0f;
    }
}

void drawGhost() {
    glPushMatrix();
    glTranslatef(ghost.x, ghost.y, ghost.z);
    
    // Configura o material da esfera
    glMaterialfv(GL_FRONT, GL_AMBIENT, ghost.matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ghost.matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ghost.matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, ghost.matShininess);
    
    glutSolidSphere(ghost.radius, 32, 32);
    glPopMatrix();
}