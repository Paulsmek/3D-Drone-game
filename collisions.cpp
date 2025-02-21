#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>
#include <glm/glm.hpp>

using namespace std;
using namespace m1;

bool Tema2::CheckCollisionWithTree(const glm::vec3& dronePosition, float droneRadius, const Obstacle& obstacle) {
    glm::vec2 droneXZ(dronePosition.x, dronePosition.z);
    glm::vec2 obstacleXZ(obstacle.position.x, obstacle.position.z);
    float horizontalDistance = glm::distance(droneXZ, obstacleXZ);

    float obstacleRadius = obstacle.base.x / 2.0f;
    bool horizontalCollision = horizontalDistance <= (droneRadius + obstacleRadius);

    float obstacleMinY = obstacle.position.y;
    float obstacleMaxY = obstacle.position.y + obstacle.height;
    float droneMinY = dronePosition.y - droneRadius;
    float droneMaxY = dronePosition.y + droneRadius;
    bool verticalCollision = !(droneMaxY < obstacleMinY || droneMinY > obstacleMaxY);

    return horizontalCollision && verticalCollision;
}

bool Tema2::CheckCollisionWithRock(const glm::vec3& dronePosition, float droneRadius, const Obstacle& obstacle) {
    if (obstacle.type != ObstacleType::ROCK) return false;

    float halfWidth = obstacle.base.x / 2.0f;
    float halfDepth = obstacle.base.y / 2.0f;

    float rectMinX = obstacle.position.x - halfWidth;
    float rectMaxX = obstacle.position.x + halfWidth;
    float rectMinZ = obstacle.position.z - halfDepth;
    float rectMaxZ = obstacle.position.z + halfDepth;

    float closestX = glm::clamp(dronePosition.x, rectMinX, rectMaxX);
    float closestZ = glm::clamp(dronePosition.z, rectMinZ, rectMaxZ);

    glm::vec2 closestPoint(closestX, closestZ);
    glm::vec2 droneXZ(dronePosition.x, dronePosition.z);
    float horizontalDistance = glm::distance(droneXZ, closestPoint);

    bool horizontalCollision = horizontalDistance <= droneRadius;

    float obstacleMinY = obstacle.position.y;
    float obstacleMaxY = obstacle.position.y + obstacle.height;
    float droneMinY = dronePosition.y - droneRadius;
    float droneMaxY = dronePosition.y + droneRadius;

    bool verticalCollision = !(droneMaxY < obstacleMinY || droneMinY > obstacleMaxY);

    return horizontalCollision && verticalCollision;
}

void Tema2::RenderTree(glm::vec3 position,const glm::vec3 scale, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    RenderSimpleMesh(meshes["cylinder"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, glm::vec3(0.749f, 0.467f, 0.047));

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position + glm::vec3(0, scale.y, 0));
    modelMatrix = glm::scale(modelMatrix, scale);
    RenderSimpleMesh(meshes["cone"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, glm::vec3(0.133f, 0.78f, 0.165f));

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position + glm::vec3(0, 1.6f * scale.y, 0));
    modelMatrix = glm::scale(modelMatrix, scale);
    RenderSimpleMesh(meshes["cone"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, glm::vec3(0.133f, 0.78f, 0.165f));
}

void Tema2::RenderPack(glm::vec3 position, glm::vec3 scale, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    RenderSimpleMesh(meshes["cubeTerrain"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, glm::vec3(0, 0, 1));

}

void Tema2::RenderTarget(glm::vec3 position, glm::vec3 scale, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    RenderSimpleMesh(meshes["cylinder"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, glm::vec3(1, 0, 0));

}

void Tema2::RenderScene(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float deltaTimeSeconds) {
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        RenderSimpleMesh(meshes["terrain"], shaders["TerrainShader"], modelMatrix, viewMatrix, projectionMatrix, glm::vec3(1, 1, 0));
    }
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, lightPosition);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderSimpleMesh(meshes["sphere"], shaders["Simple"], modelMatrix, viewMatrix, projectionMatrix);
    }
    if (viewMatrix == camera->GetViewMatrix()) {
        RenderDrone(projectionMatrix, viewMatrix, deltaTimeSeconds);
    }
    else {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(camera->GetTargetPosition().x, 1, camera->GetTargetPosition().z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.3f, 1, 1.3f));
        RenderSimpleMesh(meshes["cone"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, glm::vec3(1, 0, 0));
    }
    // draw packet
    if (!isControlled) {
        Obstacle o = Obstacle(packetPosition, ObstacleType::ROCK, 1.8f, glm::vec2(1.55f, 1.55f), glm::vec3(1, 1, 1));
        if (CheckCollisionWithRock(camera->GetTargetPosition(), 0.7f, o)) {
            isControlled = true;
            GenerateRandomPacket(100, 100, false);
        }
    }
    else {
        packetPosition = camera->GetTargetPosition() + glm::vec3 (0, -0.5f, 0);
        packetScale = glm::vec3(0.2f, 0.2f, 0.2f);
    }
    if (viewMatrix == camera->GetViewMatrix() || !isControlled) {
        RenderPack(packetPosition, packetScale, projectionMatrix, viewMatrix);
    }
    if (isControlled) {
        RenderTarget(targetPosition, glm::vec3(1, 0.3f, 1), projectionMatrix, viewMatrix);
        RenderArrow(projectionMatrix, viewMatrix, deltaTimeSeconds);
    }
    // target collision
    if (isControlled) {
        Obstacle o = Obstacle(targetPosition, ObstacleType::ROCK, 100, glm::vec2(1.55f, 1.55f), glm::vec3(1, 1, 1));
        if (CheckCollisionWithRock(camera->GetTargetPosition(), 0.7f, o)) {
            points += 1;
            cout << "Points are : " << points << "\n";
            GenerateRandomPacket(100,100,true);
            isControlled = false;
            packetScale = glm::vec3(1, 1, 1);
        }
    }

    for (const Obstacle& obs : obstacles) {
        if (obs.type == ObstacleType::TREE) {
            RenderTree(obs.position, obs.scale, projectionMatrix, viewMatrix);
            if (CheckCollisionWithTree(camera->GetTargetPosition(), 0.7f, obs))
                camera->position = oldCameraPosition;
        }
        if (obs.type == ObstacleType::ROCK) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, obs.position);
            modelMatrix = glm::scale(modelMatrix, obs.scale);
            RenderSimpleMesh(meshes["cubeTerrain"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, glm::vec3(1, 0.902f, 0.922f));

            if (CheckCollisionWithRock(camera->GetTargetPosition(), 0.7f, obs))
                camera->position = oldCameraPosition;
        }
    }
}

void Tema2::RenderDrone(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float deltaTimeSeconds) {
    glm::mat4 modelMatrix;

    glm::vec3 grayColor = glm::vec3(0.5f, 0.5f, 0.5f);

    glm::vec3 blackColor = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::mat4 baseTransformation = glm::translate(glm::mat4(1.0f), camera->GetTargetPosition());
    baseTransformation = glm::rotate(baseTransformation, droneAngle, glm::vec3(0, 1, 0));

    modelMatrix = baseTransformation;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.5f, 0.1f));
    RenderSimpleMesh(meshes["rectangle"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, grayColor);

    modelMatrix = baseTransformation;
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f, 0.5f, 0.1f));
    RenderSimpleMesh(meshes["rectangle"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, grayColor);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.55f, 0.1f, 0),
        glm::vec3(-0.55f, 0.1f, 0),
        glm::vec3(0, 0.1f, 0.55f),
        glm::vec3(0, 0.1f, -0.55f)
    };

    for (const auto& position : cubePositions) {
        modelMatrix = baseTransformation;
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
        RenderSimpleMesh(meshes["cubeDrone"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, grayColor);
    }

    float propellerHeight = 0.06f;
    glm::vec3 propellerSize = glm::vec3(0.07f, 0.1f, 0.01f);

    angularStep += 200 * deltaTimeSeconds;
    if (angularStep > 360) angularStep = 0;
    for (const auto& position : cubePositions) {
        modelMatrix = baseTransformation;
        modelMatrix = glm::translate(modelMatrix, position + glm::vec3(0, propellerHeight, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(angularStep), glm::vec3(0, 1, 0));
        modelMatrix = glm::scale(modelMatrix, propellerSize);
        RenderSimpleMesh(meshes["rectangle"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, blackColor);
    }
}

void Tema2::AddRandomObstacles(int numObstacles, int m, int n) {
    std::vector<Obstacle> obstacles;
    float minDistance = 2.5f;

    std::random_device rd;
    std::mt19937 gen(rd());
    float x = 3.0f;
    std::uniform_real_distribution<> disX(-m / x, m / x);
    std::uniform_real_distribution<> disZ(-n / x, n / x);
    std::uniform_real_distribution<> scaleFactorCube(0.8f, 2.0f);
    std::uniform_real_distribution<> scaleFactorTree(1.4f, 4.5f);
    std::uniform_real_distribution<> scaleFactorTree2(0.5f, 1.1f);

    for (int i = 0; i < numObstacles; ++i) {
        glm::vec3 position;
        bool validPosition = false;

        while (!validPosition) {
            float randX = disX(gen);
            float randZ = disZ(gen);

            float height = GetTerrainHeightAtPosition(glm::vec3(randX, 0, randZ), frequency, amplitude);
            position = glm::vec3(randX, height, randZ);

            validPosition = true;
            for (const auto& obstacle : obstacles) {
                float distance = glm::length(position - obstacle.position);
                if (distance < minDistance || (position.x > -3 && position.x < 3 && position.z > -3 && position.z < 3)) {
                    validPosition = false;
                    break;
                }
            }
        }

        ObstacleType type = (rand() % 2 == 0) ? ObstacleType::TREE : ObstacleType::ROCK;
        float height;
        glm::vec2 base;
        glm::vec3 scale;

        if (type == ObstacleType::TREE) {
            height = 2.9f;
            base = glm::vec2(2.0f, 2.0f);
            scale = glm::vec3(scaleFactorTree2(gen), scaleFactorTree(gen), 1);
        }
        else { // ROCK
            height = 1.8f;
            base = glm::vec2(1.55f, 1.55f);
            scale = glm::vec3(scaleFactorCube(gen), scaleFactorTree(gen), scaleFactorCube(gen));
        }
        height *= scale.y;
        base.x *= scale.x;
        base.y *= scale.z;
        obstacles.push_back({ position, type, height, base, scale });
    }

    this->obstacles = obstacles;
}

static float noise(const glm::vec2& position) {
    return glm::mix(glm::fract(sin(glm::dot(position, glm::vec2(12.9898f, 78.233f))) * 458.5453f), 0.1f, 0.05f);
}


float Tema2::GetTerrainHeightAtPosition(const glm::vec3& position, float frequency, float amplitude) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, position);
    glm::vec4 gl_Position = camera->GetViewMatrix() * projectionMatrix * modelMatrix * glm::vec4(position, 1.0);

    glm::vec2 noise_position = glm::vec2(gl_Position.x, gl_Position.z) * frequency;
    float noise_value = noise(noise_position);

    return noise_value * amplitude;
}

void Tema2::PreventCameraFromPassingThroughTerrain(float frequency, float amplitude) {
    glm::vec3 targetPosition = camera->GetTargetPosition();

    float terrainHeight = GetTerrainHeightAtPosition(targetPosition, frequency, amplitude);
    float terrainHeight2 = GetTerrainHeightAtPosition(camera->position, frequency, amplitude);
    if (targetPosition.y <= terrainHeight + 0.5f) {
        camera->position = oldCameraPosition;
    }
    if (camera->position.y <= terrainHeight2) {
        camera->position = oldCameraPosition;
    }
}
