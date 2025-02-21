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

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    GLint loc_light_position = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(loc_light_position, 1, glm::value_ptr(lightPosition));


    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();

    GLint loc_eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(loc_eye_position, 1, glm::value_ptr(eyePosition));


    GLint loc_material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(loc_material_kd, materialKd);

    GLint loc_material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(loc_material_ks, materialKs);

    GLint loc_material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(loc_material_shininess, materialShininess);

    GLint loc_object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(loc_object_color, 1, glm::value_ptr(color));

    GLint loc_frequency = glGetUniformLocation(shader->program, "frequency");
    glUniform1f(loc_frequency, frequency);

    GLint loc_amplitude = glGetUniformLocation(shader->program, "amplitude");
    glUniform1f(loc_amplitude, amplitude);

    GLint loc_shadow = glGetUniformLocation(shader->program, "isShadow");
    glUniform1f(loc_shadow, true);

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    mesh->Render();
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    // ========================================================================

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}

void Tema2::CreateTerrain(int m, int n) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float offsetX = -(float)m / 2.0f;
    float offsetZ = -(float)n / 2.0f;

    for (int i = 0; i <= m; ++i) {
        for (int j = 0; j <= n; ++j) {
            glm::vec3 position = glm::vec3((float)j + offsetX, 0.0f, (float)i + offsetZ);

            glm::vec3 normal = glm::vec3(0, 1, 0);
            glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);

            vertices.emplace_back(position, normal, color);
        }
    }

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            int topLeft = i * (n + 1) + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * (n + 1) + j;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    CreateMesh("terrain", vertices, indices);
    AddRandomObstacles(40, m, n);
}

void Tema2::CreateCylinder(float radius, float height, int segments) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 baseCenter(0, 0, 0);
    vertices.push_back(VertexFormat(baseCenter, glm::vec3(1, 1, 1)));

    glm::vec3 topCenter(0, height, 0);
    vertices.push_back(VertexFormat(topCenter, glm::vec3(1, 1, 1)));

    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / segments;

        glm::vec3 basePoint = glm::vec3(
            radius * cos(angle),
            0,
            radius * sin(angle)
        );
        vertices.push_back(VertexFormat(basePoint, glm::vec3(1, 0, 0)));

        // Top circle vertices
        glm::vec3 topPoint = glm::vec3(
            radius * cos(angle),
            height,
            radius * sin(angle)
        );
        vertices.push_back(VertexFormat(topPoint, glm::vec3(0, 1, 0)));
    }

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;

        indices.push_back(0);
        indices.push_back(2 + (i * 2));
        indices.push_back(2 + ((i + 1) * 2) % (segments * 2));
    }

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;

        indices.push_back(1);
        indices.push_back(2 + (i * 2) + 1);
        indices.push_back(2 + ((i + 1) * 2) % (segments * 2) + 1);
    }

    for (int i = 0; i < segments; ++i) {
        int baseIdx = 2 + (i * 2);
        int topIdx = 2 + (i * 2) + 1;
        int nextBaseIdx = 2 + ((i + 1) * 2) % (segments * 2);
        int nextTopIdx = 2 + ((i + 1) * 2) % (segments * 2) + 1;

        indices.push_back(baseIdx);
        indices.push_back(topIdx);
        indices.push_back(nextBaseIdx);

        indices.push_back(topIdx);
        indices.push_back(nextTopIdx);
        indices.push_back(nextBaseIdx);
    }

    CreateMesh("cylinder", vertices, indices);
}

void Tema2::CreateCone(float radius, float height, int segments) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 baseCenter(0, 0, 0);
    glm::vec3 apex(0, height, 0);

    vertices.push_back(VertexFormat(baseCenter, glm::vec3(1, 1, 1)));
    vertices.push_back(VertexFormat(apex, glm::vec3(0, 1, 0)));

    // Generate vertices around the base
    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / segments;

        glm::vec3 basePoint = glm::vec3(
            radius * cos(angle),
            0,
            radius * sin(angle)
        );

        vertices.push_back(VertexFormat(basePoint, glm::vec3(1, 0, 0)));
    }

    for (int i = 0; i < segments; ++i) {
        indices.push_back(0);
        indices.push_back(2 + i);
        indices.push_back(2 + (i + 1) % segments);
    }

    for (int i = 0; i < segments; ++i) {
        indices.push_back(1);
        indices.push_back(2 + i);
        indices.push_back(2 + (i + 1) % segments);
    }

    CreateMesh("cone", vertices, indices);
}

void Tema2::SetupMinimapCamera() {
    glm::vec3 dronePosition = camera->GetTargetPosition();
    minimapCamera->Set(dronePosition + glm::vec3(0, 10.0f, 0), glm::vec3(0, 5, 0), glm::vec3(0, 1, 0));
}

void Tema2::UpdateMinimapCamera() {
    minimapCamera->position = camera->GetTargetPosition();
    minimapCamera->up = glm::normalize(camera->forward);
}

void Tema2::GenerateRandomPacket(int m, int n, bool ok) {
    bool validPosition = false;
    float minDistance = 2.5;
    std::random_device rd;
    std::mt19937 gen(rd());
    float x = 3.0f;
    std::uniform_real_distribution<> disX(-m / x, m / x);
    std::uniform_real_distribution<> disZ(-n / x, n / x);

    glm::vec3 p = packetPosition;
    while (!validPosition) {
        float randX = disX(gen);
        float randZ = disZ(gen);

        float height = GetTerrainHeightAtPosition(glm::vec3(randX, 0, randZ), frequency, amplitude);
        packetPosition = glm::vec3(randX, height, randZ);
        validPosition = true;
        for (const auto& obstacle : obstacles) {
            float distance = glm::length(packetPosition - obstacle.position);
            if (distance < minDistance) {
                validPosition = false;
                break;
            }
        }
    }
    if (!ok) {
        targetPosition = packetPosition;
        packetPosition = p;
    }
}

void Tema2::RenderArrow(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float deltaTimeSeconds) {
    glm::vec3 cameraPosition = camera->GetTargetPosition();

    glm::vec2 targetXZ(targetPosition.x, targetPosition.z);
    glm::vec2 cameraXZ(cameraPosition.x, cameraPosition.z);

    glm::vec2 direction = glm::normalize(targetXZ - cameraXZ);

    float angleY = atan2(direction.y, direction.x);

    glm::mat4 modelMatrix = glm::mat4(1.0f); 

    modelMatrix = glm::translate(modelMatrix, glm::vec3(cameraPosition.x, 1.0f, cameraPosition.z));
    modelMatrix = glm::rotate(modelMatrix, 300 - angleY, glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));

    if (viewMatrix == camera->GetViewMatrix()) {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2, 2, 0.2));
    }
    else {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.9f, 6, 0.9f));
    }
    RenderSimpleMesh(meshes["cone"], shaders["LabShader"], modelMatrix, viewMatrix, projectionMatrix, glm::vec3(1, 0, 0));
}
