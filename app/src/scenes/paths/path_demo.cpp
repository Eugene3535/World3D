#include <array>
#include <memory>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/Image.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "data/AppData.hpp"


int path_demo(GLFWwindow* window)
{
    glEnable(GL_DEPTH_TEST);

    auto isKeyPressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    AppData* appData = static_cast<AppData*>(glfwGetWindowUserPointer(window));

    Image imgSnow;     imgSnow.loadFromFile("res/textures/snow.png");
    Image imgPavement; imgPavement.loadFromFile("res/textures/pavement.jpg");
    Image imgPath;     imgPath.loadFromFile("res/textures/test.png");

    std::array<uint32_t, 3> textures = appData->resourceHolder.create<Texture2D, 3>();

    auto texSnow = std::make_unique<Texture2D>(textures[0]);
    auto texPavement = std::make_unique<Texture2D>(textures[1]);
    auto texPath = std::make_unique<Texture2D>(textures[2]);

    texSnow->loadFromImage(imgSnow, true, true);
    texPavement->loadFromImage(imgPavement, true, true);
    texPath->loadFromImage(imgPath, true, true);

    float mapWidth  = static_cast<float>(imgPath.getWidth());
    float mapHeight = static_cast<float>(imgPath.getHeight());

    std::array<float, 20> vertices = 
    {
        0.0f,     0.0f, 0.0f,      0.0f,  0.0f,
        mapWidth, 0.0f, 0.0f,      10.0f, 0.0f,
        mapWidth, 0.0f, mapHeight, 10.0f, 10.0f,
        0.0f,     0.0f, mapHeight, 0.0f,  10.0f
    };

    std::array<VertexBufferLayout::Attribute, 2> attributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };
    VertexBufferLayout layout(attributes);
    std::array<uint32_t, 2> buffers = appData->resourceHolder.create<GlBuffer, 2>();
    std::array<uint32_t, 1> vertexArrays = appData->resourceHolder.create<VertexArrayObject, 1>();

    GlBuffer vbo(buffers[0], GL_ARRAY_BUFFER);
    vbo.create(sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

    auto vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    vao->addVertexBuffer(vbo, layout);

    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile("res/shaders/ground.vert", GL_VERTEX_SHADER)) return -1;
    if(!shaders[1].loadFromFile("res/shaders/ground.frag", GL_FRAGMENT_SHADER)) return -1;

    auto program = std::make_unique<ShaderProgram>();
    if(!program->link(shaders)) return -1;

    glUseProgram(program->getHandle().value());
    glUniform1i(program->getUniformLocation("snowSampler").value(), 0);
    glUniform1i(program->getUniformLocation("pavementSampler").value(), 1);
    glUniform1i(program->getUniformLocation("pathSampler").value(), 2);
    glUseProgram(0);

    GlBuffer uniformBuffer(buffers[1], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = &appData->camera.perspective;
    camera->setupProjectionMatrix(45, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    camera->setPosition(3, 3, 3);

    while (!glfwWindowShouldClose(window))
    {
        if(isKeyPressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, true);
            continue;
        }
        
        if(isKeyPressed(GLFW_KEY_W))
            camera->moveForward(100);

        if(isKeyPressed(GLFW_KEY_A))
            camera->moveLeft(100);

        if(isKeyPressed(GLFW_KEY_S))
            camera->moveBackward(100);

        if(isKeyPressed(GLFW_KEY_D))
            camera->moveRight(100);

        auto playerPos = camera->getPosition();
        playerPos.y = 30.7f;
        camera->setPosition(playerPos);

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int xt, yt;
        glfwGetWindowPos(window, &xt, &yt);

        glfwGetWindowSize(window, &width, &height);

        xt += width >> 1;
        yt += height >> 1;

        camera->rotateX((xt - xpos) * 0.125f);
        camera->rotateY((yt - ypos) * 0.125f);

        glfwSetCursorPos(window, xt, yt);
        camera->apply(0.01f);
        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glUseProgram(program->getHandle().value());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texSnow->getHandle());
    
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texPavement->getHandle());
    
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texPath->getHandle());

        glBindVertexArray(vao->getHandle());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}