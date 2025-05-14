#include "structures.h"

void lighting() {
    glShadeModel(GL_SMOOTH);
    glDisable(GL_COLOR_MATERIAL);

    float ambientLight[4] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat lightSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // SpotLight configs
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0f); // Ângulo da lanterna
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 15.0f); // Concentração da luz

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void generateDisplayLists() {
    if(displayListGenerated)
        return;
    
    // Gera display list para cena interna
    displayListInside = glGenLists(1);
    glNewList(displayListInside, GL_COMPILE);
    
    glBegin(GL_TRIANGLES);
    Material* lastMaterial = nullptr;
    
    for(const auto& face : facesInside) {
        const Vertex& v1 = verticesInside[face.v1];
        const Vertex& v2 = verticesInside[face.v2];
        const Vertex& v3 = verticesInside[face.v3];
        
        if(face.material != lastMaterial) {
            if(face.material) {
                glMaterialfv(GL_FRONT, GL_AMBIENT, face.material -> ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, face.material -> diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, face.material -> specular);
                glMaterialf(GL_FRONT, GL_SHININESS, face.material -> shininess);
            } else {
                glMaterialfv(GL_FRONT, GL_AMBIENT, defaultMaterial.ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultMaterial.diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, defaultMaterial.specular);
                glMaterialf(GL_FRONT, GL_SHININESS, defaultMaterial.shininess);
            }
            lastMaterial = face.material;
        }
        
        float nx, ny, nz;
        float ux = v2.x - v1.x, uy = v2.y - v1.y, uz = v2.z - v1.z;
        float vx = v3.x - v1.x, vy = v3.y - v1.y, vz = v3.z - v1.z;
        nx = uy * vz - uz * vy;
        ny = uz * vx - ux * vz;
        nz = ux * vy - uy * vx;
        
        float len = sqrt(nx*nx + ny*ny + nz*nz);
        
        if (len > 0)
            nx /= len; ny /= len; nz /= len;

        glNormal3f(nx, ny, nz);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
    glEndList();
    
    // Gera display list para cena externa
    displayListOutside = glGenLists(1);
    glNewList(displayListOutside, GL_COMPILE);
    
    glBegin(GL_TRIANGLES);
    lastMaterial = nullptr;
    
    for(const auto& face : facesOutside) {
        const Vertex& v1 = verticesOutside[face.v1];
        const Vertex& v2 = verticesOutside[face.v2];
        const Vertex& v3 = verticesOutside[face.v3];
        
        if(face.material != lastMaterial) {
            if(face.material) {
                glMaterialfv(GL_FRONT, GL_AMBIENT, face.material -> ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, face.material -> diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, face.material -> specular);
                glMaterialf(GL_FRONT, GL_SHININESS, face.material -> shininess);
            } else {
                glMaterialfv(GL_FRONT, GL_AMBIENT, defaultMaterial.ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultMaterial.diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, defaultMaterial.specular);
                glMaterialf(GL_FRONT, GL_SHININESS, defaultMaterial.shininess);
            }
            lastMaterial = face.material;
        }
        
        float nx, ny, nz;
        float ux = v2.x - v1.x, uy = v2.y - v1.y, uz = v2.z - v1.z;
        float vx = v3.x - v1.x, vy = v3.y - v1.y, vz = v3.z - v1.z;
        nx = uy * vz - uz * vy;
        ny = uz * vx - ux * vz;
        nz = ux * vy - uy * vx;
        
        float len = sqrt(nx*nx + ny*ny + nz*nz);

        if (len > 0)
            nx /= len; ny /= len; nz /= len;

        glNormal3f(nx, ny, nz);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
    glEndList();
    
    displayListGenerated = true;
}

// Inicialização do OpenGL
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(60.0f, 1.0f, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
    glutWarpPointer(centerX, centerY);

    lighting();
}

void updateLightPosition() {
    // Posição da luz (acima e um pouco à frente da câmera)
    GLfloat lightPosition[] = {camX, camY + 1.0f, camZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Direção da luz baseada no vetor de direção da câmera
    GLfloat lightDirection[] = {dirX, dirY, dirZ};
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
}

void display() {
    updateLightPosition();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ, camX + dirX, camY + dirY, camZ + dirZ, 0.0f, 1.0f, 0.0f);

    if (!displayListGenerated) {
        generateDisplayLists();
    }
    
    if (inside) {
        glCallList(displayListInside);
        
        // Desenha os 3 fantasmas
        for(int i = 0; i < 3; i++) {
            Ghost& ghost = (i == 0) ? ghost1 : (i == 1) ? ghost2 : ghost3;
            
            glPushMatrix();
            glTranslatef(ghost.x, ghost.y, ghost.z);
            glScalef(scaleGhost, scaleGhost, scaleGhost); // Escala fixa para todos
            
            // Configura material
            glMaterialfv(GL_FRONT, GL_AMBIENT, ghost.matAmbient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, ghost.matDiffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, ghost.matSpecular);
            glMaterialf(GL_FRONT, GL_SHININESS, ghost.matShininess);
            
            // Desenha o modelo
            glBegin(GL_TRIANGLES);
            for(const auto& face : ghostFaces) {
                const Vertex& v1 = ghostVertices[face.v1];
                const Vertex& v2 = ghostVertices[face.v2];
                const Vertex& v3 = ghostVertices[face.v3];
                
                // Calcula normal
                float nx = (v2.y-v1.y)*(v3.z-v1.z) - (v2.z-v1.z)*(v3.y-v1.y);
                float ny = (v2.z-v1.z)*(v3.x-v1.x) - (v2.x-v1.x)*(v3.z-v1.z);
                float nz = (v2.x-v1.x)*(v3.y-v1.y) - (v2.y-v1.y)*(v3.x-v1.x);
                float len = sqrt(nx*nx + ny*ny + nz*nz);
                
                if(len > 0) { nx /= len; ny /= len; nz /= len; }
                
                glNormal3f(nx, ny, nz);
                glVertex3f(v1.x, v1.y, v1.z);
                glVertex3f(v2.x, v2.y, v2.z);
                glVertex3f(v3.x, v3.y, v3.z);
            }
            glEnd();
            glPopMatrix();
        }
    } else {
        glCallList(displayListOutside);
    }
    
    glutSwapBuffers();
}

void reshape(int width, int height) {
    if(height == 0)
        height = 1;
    float ratio = width * 1.0f / height;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Limpeza de recursos
void cleanup() {
    if(displayListGenerated) {
        glDeleteLists(displayListInside, 1);
        glDeleteLists(displayListOutside, 1);
    }
}
