#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{   
    polygonMode = GL_FILL;
    camera = new implemented2::Camera();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(glm::radians(FoV), window->props.aspectRatio, 1.0f, 200.0f);

    minimapCamera = new implemented2::Camera();
    minimapCamera->Set(camera->position + glm::vec3(0, 20.0f, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    float aspectRatio = window->props.aspectRatio;
    projectionMatrixMinimap = glm::ortho(-orthoWidth * aspectRatio, orthoWidth * aspectRatio, -orthoHeight, orthoHeight, 0.01f, 200.0f);

    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(1, -1,  1), glm::vec3(1, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3(1,  1, -1), glm::vec3(0, 0, 1), glm::vec3(0.1, 0.5, 0.8)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };

        CreateMesh("cubeDrone", vertices, indices);
    }
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, 0,  1), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(1, 0,  1), glm::vec3(1, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-1, 2,  1), glm::vec3(1, 0, 0), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3(1, 2,  1), glm::vec3(0, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-1, 0, -1), glm::vec3(1, 1, 1), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3(1, 0, -1), glm::vec3(0, 1, 1), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-1, 2, -1), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3(1, 2, -1), glm::vec3(0, 0, 1), glm::vec3(0.1, 0.5, 0.8)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };

        CreateMesh("cubeTerrain", vertices, indices);
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-2, -0.1, -0.5), glm::vec3(1, 0, 0), glm::vec3(0.8, 0.2, 0.2)),
            VertexFormat(glm::vec3(2, -0.1, -0.5), glm::vec3(1, 0, 0), glm::vec3(0.8, 0.2, 0.2)),
            VertexFormat(glm::vec3(-2, 0.1, -0.5), glm::vec3(1, 0, 0), glm::vec3(0.8, 0.2, 0.2)),
            VertexFormat(glm::vec3(2, 0.1, -0.5), glm::vec3(1, 0, 0), glm::vec3(0.8, 0.2, 0.2)),

            VertexFormat(glm::vec3(-2, -0.1, 0.5), glm::vec3(1, 0, 0), glm::vec3(0.8, 0.2, 0.2)),
            VertexFormat(glm::vec3(2, -0.1, 0.5), glm::vec3(1, 0, 0), glm::vec3(0.8, 0.2, 0.2)),
            VertexFormat(glm::vec3(-2, 0.1, 0.5), glm::vec3(1, 0, 0), glm::vec3(0.8, 0.2, 0.2)),
            VertexFormat(glm::vec3(2, 0.1, 0.5), glm::vec3(1, 0, 0), glm::vec3(0.8, 0.2, 0.2)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,  1, 3, 2,

            4, 5, 6,  5, 7, 6,

            2, 3, 6,  3, 7, 6,

            0, 1, 4,  1, 5, 4,

            0, 2, 4,  2, 6, 4,

            1, 3, 5,  3, 7, 5,
        };

        CreateMesh("rectangle", vertices, indices);
    }
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader2.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader2.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("TerrainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShaderNoise.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShaderNoise.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    CreateCylinder(0.8f, 2, 100);
    CreateCone(2, 1.8f, 100);
    CreateTerrain(100, 100);
    GenerateRandomPacket(100, 100, true);
    {
        lightPosition = glm::vec3(30, 50, 30);
        materialShininess = 100;
        materialKd = 0.5;
        materialKs = 0.5;
    }

}


void Tema2::FrameStart()
{
    glClearColor(0.2f, 0.4f, 0.7f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}


void Tema2::Update(float deltaTimeSeconds)
{

    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    RenderScene(projectionMatrix, camera->GetViewMatrix(), deltaTimeSeconds);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width * resolution.x / 1000, miniViewportArea.height * resolution.y / 500);

    UpdateMinimapCamera();
    RenderScene(projectionMatrixMinimap, minimapCamera->GetViewMatrix(), deltaTimeSeconds);
}


void Tema2::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    oldCameraPosition = camera->position;
    if (true)
    {
        float cameraSpeed = 10.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            camera->TranslateForward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            camera->TranslateRight(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            camera->TranslateForward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            camera->TranslateRight(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            camera->TranslateUpward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera->TranslateUpward(cameraSpeed * deltaTime);
        }
    }
    PreventCameraFromPassingThroughTerrain(frequency, amplitude);
}

void Tema2::OnKeyPress(int key, int mods)
{
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            if(camera->position.y > 1 || deltaY > 0)
                camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOY);
            droneAngle += -deltaX * sensivityOY;
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
