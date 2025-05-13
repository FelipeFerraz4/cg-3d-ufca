#include "structures.h"

void normalizeHorizontal(float& x, float& z) {
    float length = sqrt(x * x + z * z);
    if (length > 0) {
        x /= length;
        z /= length;
    }
}

void updateCamera() {
    dirX = sin(angleY * M_PI / 180) * cos(angleX * M_PI / 180);
    dirY = sin(angleX * M_PI / 180);
    dirZ = -cos(angleY * M_PI / 180) * cos(angleX * M_PI / 180);
    glutPostRedisplay();
}

float clamp(float val, float minVal, float maxVal) {
    return max(minVal, min(maxVal, val));
}

float pointToTriangleDistance(float px, float py, float pz, const Vertex& v0, const Vertex& v1, const Vertex& v2) {
    float v0x = v1.x - v0.x;
    float v0y = v1.y - v0.y;
    float v0z = v1.z - v0.z;

    float v1x = v2.x - v0.x;
    float v1y = v2.y - v0.y;
    float v1z = v2.z - v0.z;

    float vx = px - v0.x;
    float vy = py - v0.y;
    float vz = pz - v0.z;

    float d00 = v0x*v0x + v0y*v0y + v0z*v0z;
    float d01 = v0x*v1x + v0y*v1y + v0z*v1z;
    float d11 = v1x*v1x + v1y*v1y + v1z*v1z;
    float d20 = vx*v0x + vy*v0y + vz*v0z;
    float d21 = vx*v1x + vy*v1y + vz*v1z;

    float denom = d00 * d11 - d01 * d01;
    float u = (d11 * d20 - d01 * d21) / denom;
    float v = (d00 * d21 - d01 * d20) / denom;

    if (u >= 0 && v >= 0 && (u + v) <= 1.0f) {
        float projX = v0.x + u * v0x + v * v1x;
        float projY = v0.y + u * v0y + v * v1y;
        float projZ = v0.z + u * v0z + v * v1z;
        float dx = px - projX;
        float dy = py - projY;
        float dz = pz - projZ;
        return sqrt(dx*dx + dy*dy + dz*dz);
    }

    auto pointToSegmentDistance = [](float px, float py, float pz, const Vertex& a, const Vertex& b) {
        float abx = b.x - a.x;
        float aby = b.y - a.y;
        float abz = b.z - a.z;

        float apx = px - a.x;
        float apy = py - a.y;
        float apz = pz - a.z;

        float abLenSq = abx*abx + aby*aby + abz*abz;
        float t = (abx*apx + aby*apy + abz*apz) / abLenSq;
        t = clamp(t, 0.0f, 1.0f);

        float cx = a.x + t * abx;
        float cy = a.y + t * aby;
        float cz = a.z + t * abz;

        float dx = px - cx;
        float dy = py - cy;
        float dz = pz - cz;

        return sqrt(dx*dx + dy*dy + dz*dz);
    };

    float d1 = pointToSegmentDistance(px, py, pz, v0, v1);
    float d2 = pointToSegmentDistance(px, py, pz, v1, v2);
    float d3 = pointToSegmentDistance(px, py, pz, v2, v0);

    return min(d1, min(d2, d3));
}

bool checkCollision(float newX, float newY, float newZ) {
    if (newY - EYE_HEIGHT + PLAYER_HEIGHT < 0) {
        return true;
    }
    
    const auto& collisionMesh = inside ? collisionMeshInside : collisionMeshOutside;
    
    for (const auto& face : collisionMesh.faces) {
        const Vertex& v1 = collisionMesh.vertices[face.v1];
        const Vertex& v2 = collisionMesh.vertices[face.v2];
        const Vertex& v3 = collisionMesh.vertices[face.v3];
        
        float minX = min(v1.x, min(v2.x, v3.x));
        float maxX = max(v1.x, max(v2.x, v3.x));
        float minY = min(v1.y, min(v2.y, v3.y));
        float maxY = max(v1.y, max(v2.y, v3.y));
        float minZ = min(v1.z, min(v2.z, v3.z));
        float maxZ = max(v1.z, max(v2.z, v3.z));
        
        if (newX > minX - PLAYER_RADIUS && newX < maxX + PLAYER_RADIUS &&
            newY > minY - PLAYER_HEIGHT && newY < maxY + PLAYER_HEIGHT &&
            newZ > minZ - PLAYER_RADIUS && newZ < maxZ + PLAYER_RADIUS) {
            
            float dist = pointToTriangleDistance(newX, newY, newZ, v1, v2, v3);
            if (dist < PLAYER_RADIUS) {
                return true;
            }
        }
    }
    return false;
}

bool checkCollisionWithResponse(float& newX, float& newY, float& newZ, float oldX, float oldY, float oldZ) {
    if (!checkCollision(newX, newY, newZ)) {
        return false;
    }
    
    float testX = newX;
    float testZ = oldZ;
    if (!checkCollision(testX, newY, testZ)) {
        newX = testX;
        newZ = testZ;
        return false;
    }
    
    testX = oldX;
    testZ = newZ;
    if (!checkCollision(testX, newY, testZ)) {
        newX = testX;
        newZ = testZ;
        return false;
    }
    
    newX = oldX;
    newY = oldY;
    newZ = oldZ;
    return true;
}