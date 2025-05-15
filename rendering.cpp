#include "structures.h"

GLuint backgroundTexture;

void loadTextures() {
    backgroundTexture = SOIL_load_OGL_texture(
        "imagens/background.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA
    );
    
    if(!backgroundTexture) {
        printf("Erro ao carregar textura: '%s'\n", SOIL_last_result());
    }
    
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void drawBackground() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1460, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1460, 720);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 720);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

bool isGhostHitByLight(const Ghost& ghost, float lightX, float lightY, float lightZ, float dirX, float dirY, float dirZ, float cutoffAngle, float maxDistance) // <- novo parâmetro
{
    if(!ghost.alive)
        return false;

    float dx = ghost.x - lightX;
    float dy = ghost.y - lightY;
    float dz = ghost.z - lightZ;

    float distance = sqrt(dx*dx + dy*dy + dz*dz);

    if (distance > maxDistance) return false;

    float lightDirLen = sqrt(dirX*dirX + dirY*dirY + dirZ*dirZ);
    float lx = dirX / lightDirLen;
    float ly = dirY / lightDirLen;
    float lz = dirZ / lightDirLen;

    float ghostDirLen = distance;
    float gx = dx / ghostDirLen;
    float gy = dy / ghostDirLen;
    float gz = dz / ghostDirLen;

    float dot = lx * gx + ly * gy + lz * gz;

    float angle = acos(dot) * 180.0f / M_PI;

    return angle < cutoffAngle;
}

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

    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0f); 
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 15.0f); 

    glEnable(GL_LIGHTING);

    GLfloat lightDiffuse1[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat lightSpecular1[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat lightPosition[] = {-31.3182f, 47.2501f, 11.8853f, 1.0f};

    glLightfv(GL_LIGHT1, GL_AMBIENT, black);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);

    GLfloat lightDiffuse2[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat lightSpecular2[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat lightPosition2[] = {0.0f, 5.0f, 5.0f, 1.0f};

    glLightfv(GL_LIGHT2, GL_AMBIENT, black);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT2, GL_POSITION, lightPosition);

    if(inside) {
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        //glEnable(GL_LIGHT2);
    } else {
        glDisable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        //glDisable(GL_LIGHT2);
    }
}

void generateDisplayLists() {
    if(displayListGenerated)
        return;
    
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
        
        if(len > 0)
            nx /= len; ny /= len; nz /= len;

        glNormal3f(nx, ny, nz);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
    glEndList();
    
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
                if(face.name == "Material_0.080") {
                    GLfloat emissionColor[] = {1.0f, 1.0f, 0.8f, 1.0f};  
                    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissionColor);
                } else {
                    GLfloat noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};   
                    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
                }
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

        if(len > 0)
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

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    loadTextures();

    if(aliveGhosts <= 0){
        loadTextures();
        glEnable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        gluPerspective(60.0f, 1.0f, 1.0f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
        
        centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
        centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
        glutWarpPointer(centerX, centerY);
    }else{
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        gluPerspective(60.0f, 1.0f, 1.0f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
        
        centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
        centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

        glutWarpPointer(centerX, centerY);
    }
    loadTextures();
}

void updateLightPosition() {
    GLfloat lightPosition[] = {camX, camY + 1.0f, camZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    GLfloat lightDirection[] = {dirX, dirY, dirZ};
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
}

void display() {
    
    if(aliveGhosts <= 0){
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 1460, 720, 0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        glEnable(GL_TEXTURE_2D);
        drawBackground();
        glDisable(GL_TEXTURE_2D);
        
        glutSwapBuffers();
    }else{
        updateLightPosition();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(camX, camY, camZ, camX + dirX, camY + dirY, camZ + dirZ, 0.0f, 1.0f, 0.0f);

        if(!displayListGenerated) {
            generateDisplayLists();
        }
        
        if(inside) {
            glCallList(displayListInside);
            
        
            for(int i = 0; i < qtdGhosts; i++) {
                Ghost& ghost = ghosts[i];
                
                if(ghost.alive) {
                    glPushMatrix();
                    glTranslatef(ghost.x, ghost.y, ghost.z);
                    glScalef(scaleGhost, scaleGhost, scaleGhost);
        
                    glMaterialfv(GL_FRONT, GL_AMBIENT, ghost.matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, ghost.matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, ghost.matSpecular);
                    glMaterialf(GL_FRONT, GL_SHININESS, ghost.matShininess);
                    
                    glBegin(GL_TRIANGLES);
                    for(const auto& face : ghostFaces) {
                        const Vertex& v1 = ghostVertices[face.v1];
                        const Vertex& v2 = ghostVertices[face.v2];
                        const Vertex& v3 = ghostVertices[face.v3];
                        
                        float nx = (v2.y-v1.y)*(v3.z-v1.z) - (v2.z-v1.z)*(v3.y-v1.y);
                        float ny = (v2.z-v1.z)*(v3.x-v1.x) - (v2.x-v1.x)*(v3.z-v1.z);
                        float nz = (v2.x-v1.x)*(v3.y-v1.y) - (v2.y-v1.y)*(v3.x-v1.x);
                        float len = sqrt(nx*nx + ny*ny + nz*nz);
                        
                        if(len > 0) {
                            nx /= len;
                            ny /= len;
                            nz /= len;
                        }
                        
                        glNormal3f(nx, ny, nz);
                        glVertex3f(v1.x, v1.y, v1.z);
                        glVertex3f(v2.x, v2.y, v2.z);
                        glVertex3f(v3.x, v3.y, v3.z);
                    }
                    glEnd();
                    glPopMatrix();
                }
            }

            GLfloat lightX = camX;
            GLfloat lightY = camY + 1.0f;
            GLfloat lightZ = camZ;

            for(int i = 0; i < qtdGhosts; i++) {
                Ghost& ghost = ghosts[i];
                
                bool hit = isGhostHitByLight(ghost, lightX, lightY, lightZ, dirX, dirY, dirZ, 30.0f, 5.0f);

                if(hit) {
                    ghosts[i].alive = false;
                    aliveGhosts--;
                    printf("Fantasma %d foi atingido pela luz!\n", i+1);
                    printf("Fantasmas vivos: %d\n", aliveGhosts);
                }
            }
        } else {
            glCallList(displayListOutside);
        }


        isPlayerHitByGhost();
        cout << "Tá vivo? " << playerIsAlive << endl;
        cout << "Fantasmas " << aliveGhosts << endl;

        if(aliveGhosts == 0) {
            inside = false;
            camX = -16.093f;
            camY = 1.40001f;
            camZ = -0.982863f;
            dirX = 0.997792f; dirY = -0.0262446f;
            dirZ = 0.0610081f;
            aliveGhosts--;
        }

        if(!playerIsAlive)
            exit(1);

        lighting();
        glutSwapBuffers();
    }
    
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

void cleanup() {
    if(displayListGenerated) {
        glDeleteLists(displayListInside, 1);
        glDeleteLists(displayListOutside, 1);
    }
}
