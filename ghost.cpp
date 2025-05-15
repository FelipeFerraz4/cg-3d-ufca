#include "structures.h"
#include <cstdlib>
#include <ctime>

int initGhost() {
    // Inicializa o gerador de números aleatórios
    static bool initialized = false;
    if (!initialized) {
        srand(static_cast<unsigned>(time(nullptr)));
        initialized = true;
    }

    for(int i = 0; i < 10; i++) {
        ghosts[i] = {
            .x = 45.0f, .y = 7.5f, .z = 10.0f,
            .dirX = 0.05f, .dirY = 0.05f, .dirZ = 0.05f,
            .radius = 0.5f,
            .matAmbient = {0.7f, 0.7f, 0.7f, 1.0f},
            .matDiffuse = {0.8f, 0.8f, 0.8f, 1.0f},
            .matSpecular = {1.0f, 1.0f, 1.0f, 1.0f},
            .matShininess = 50.0f,
            .alive = true
        };
    }
    return 1;
}

void updateGhost(Ghost &ghost) {
    // Atualiza a posição com base na direção atual
    ghost.x += ghost.dirX;
    ghost.y += ghost.dirY;
    ghost.z += ghost.dirZ;
    
    // Limites da cena (ajustados para o ambiente do jogo)
    const float minX = 20.0f, maxX = 80.0f;
    const float minY = 3.0f, maxY = 10.0f;
    const float minZ = -14.0f, maxZ = 25.0f;
    
    // Verifica colisões com os limites e inverte direção
    if(ghost.x > maxX || ghost.x < minX) {
        ghost.dirX = -ghost.dirX * 0.9f; // Inverte com pequena perda de energia
        ghost.x = clamp(ghost.x, minX, maxX); // Corrige posição
    }
    if(ghost.y > maxY || ghost.y < minY) {
        ghost.dirY = -ghost.dirY * 0.9f;
        ghost.y = clamp(ghost.y, minY, maxY);
    }
    if(ghost.z > maxZ || ghost.z < minZ) {
        ghost.dirZ = -ghost.dirZ * 0.9f;
        ghost.z = clamp(ghost.z, minZ, maxZ);
    }
    
    // Mudança aleatória de direção (5% de chance por frame)
    if(rand() % 100 < 5) {
        ghost.dirX += (rand() % 100 - 50) / 500.0f;
        ghost.dirY += (rand() % 100 - 50) / 500.0f;
        ghost.dirZ += (rand() % 100 - 50) / 500.0f;
        
        // Normaliza a velocidade para manter movimento constante
        float speed = sqrt(ghost.dirX*ghost.dirX + ghost.dirY*ghost.dirY + ghost.dirZ*ghost.dirZ);
        float desiredSpeed = 0.1f; // Velocidade base
        ghost.dirX = ghost.dirX / speed * desiredSpeed;
        ghost.dirY = ghost.dirY / speed * desiredSpeed;
        ghost.dirZ = ghost.dirZ / speed * desiredSpeed;
    }
}

void drawGhost(Ghost &ghost) {
    glPushMatrix();
    glTranslatef(ghost.x, ghost.y, ghost.z);
    glScalef(0.1f, 0.1f, 0.1f); // Ajuste este valor conforme necessário
    
    // Configura material
    glMaterialfv(GL_FRONT, GL_AMBIENT, ghost.matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ghost.matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ghost.matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, ghost.matShininess);
    
    // Desenha o modelo
    glBegin(GL_TRIANGLES);
    for(const auto& face : ghostFaces) {
        if(face.v1 >= ghostVertices.size() || face.v2 >= ghostVertices.size() || face.v3 >= ghostVertices.size()) {
            cerr << "Índice de vértice inválido!" << endl;
            continue;
        }
        
        const Vertex& v1 = ghostVertices[face.v1];
        const Vertex& v2 = ghostVertices[face.v2];
        const Vertex& v3 = ghostVertices[face.v3];
        
        // Calcula normal
        float ux = v2.x - v1.x, uy = v2.y - v1.y, uz = v2.z - v1.z;
        float vx = v3.x - v1.x, vy = v3.y - v1.y, vz = v3.z - v1.z;
        float nx = uy * vz - uz * vy;
        float ny = uz * vx - ux * vz;
        float nz = ux * vy - uy * vx;
        float len = sqrt(nx*nx + ny*ny + nz*nz);
        
        if (len > 0) {
            nx /= len; ny /= len; nz /= len;
        }

        glNormal3f(nx, ny, nz);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
    
    glPopMatrix();
}

void isPlayerHitByGhost() {
    for(int i = 0; i < qtdGhosts; i++) {
        if(ghosts[i].alive) {
            if(sqrt(ghosts[i].x * ghosts[i].x  + ghosts[i].y * ghosts[i].y + ghosts[i].z * ghosts[i].z) < 1) {
                playerIsAlive = false;
                break;
            }
        }
    }
}