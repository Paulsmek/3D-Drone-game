#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"
#include "lab_m1/tema2/lab_camera2.h"

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
         struct ViewportArea
         {
             ViewportArea() : x(0), y(0), width(1), height(1) {}
             ViewportArea(int x, int y, int width, int height)
                 : x(x), y(y), width(width), height(height) {}
             int x;
             int y;
             int width;
             int height;
         };
         Tema2();
        ~Tema2();

        void Init() override;

        enum class ObstacleType {
            TREE,
            ROCK,
            WATER,
            // Add other types as needed
        };

        struct Obstacle {
            glm::vec3 position;  // Position of the obstacle in 3D space
            ObstacleType type;    // Type of the obstacle
            float height;
            glm::vec2 base;
            glm::vec3 scale;
            // Constructor to initialize the obstacle
            Obstacle(const glm::vec3& pos, ObstacleType t, float hei, glm::vec2 b, glm::vec3 s)
                : position(pos), type(t), height(hei), base(b), scale(s) {}
        };
     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec3 &color = glm::vec3(1));

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void RenderDrone(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float deltaTimeSeconds);
        void CreateCylinder(float radius, float height, int segments);
        void CreateCone(float radius, float height, int segments);
        void RenderTree(glm::vec3 position, glm::vec3 scale, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
        void CreateTerrain(int m, int n);
        float GetTerrainHeightAtPosition(const glm::vec3& position, float frequency, float amplitude);
        void PreventCameraFromPassingThroughTerrain(float frequency, float amplitude);
        void AddRandomObstacles(int numObstacles, int m, int n);
        bool CheckCollisionWithTree(const glm::vec3& dronePosition, float droneRadius, const Obstacle& obstacle);
        bool CheckCollisionWithRock(const glm::vec3& dronePosition, float droneRadius, const Obstacle& obstacle);
        void RenderScene(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float deltaTimeSeconds);
        void SetupMinimapCamera();
        void UpdateMinimapCamera();
        void GenerateRandomPacket(int m, int n, bool ok);
        void RenderPack(glm::vec3 position, glm::vec3 scale, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
        void RenderTarget(glm::vec3 position, glm::vec3 scale, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
        void RenderArrow(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float deltaTimeSeconds);

        glm::vec3 lightPosition;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;
        float frequency = 0.05f;
        float amplitude = 1.1f;
    protected:
        implemented2::Camera* camera;
        implemented2::Camera* minimapCamera;
        glm::mat4 projectionMatrix;
        glm::mat4 projectionMatrixMinimap;

        bool isPerspective = true;
        float FoV = 60.0f;
        float orthoWidth = 10.0f;
        float orthoHeight = 10.0f;

        float angularStep = 0;
        float droneAngle = RADIANS(45);
        glm::vec3 dronePosition = glm::vec3(0, 2, 3.5f);

        glm::vec3 oldCameraPosition;
        std::vector<Obstacle> obstacles;
        ViewportArea miniViewportArea;
        GLenum polygonMode;

        glm::vec3 packetPosition;
        glm::vec3 targetPosition;
        glm::vec3 packetScale = glm::vec3 (1,1,1);
        bool isControlled = false;
        float points = 0;

    };
}   // namespace m1
