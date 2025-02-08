#include <array>
#include <memory>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "files/Image.hpp"
#include "opengl/resources/shaders/ShaderProgram.hpp"
#include "opengl/holder/GlResourceHolder.hpp"
#include "camera/orthogonal/Orthogonal.hpp"

void dune_demo(GLFWwindow* window)
{
    auto isKeyPressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    Image imgMask;
    Image imgSand;
    Image imgSpace;
    Image imgRock;
    Image imgStone;

    if(!imgMask.loadFromFile("res/textures/mask.png"))   return;
    if(!imgSand.loadFromFile("res/textures/sand.jpg"))   return;
    if(!imgSpace.loadFromFile("res/textures/spice.jpg")) return;
    if(!imgRock.loadFromFile("res/textures/rock01.jpg")) return;
    if(!imgStone.loadFromFile("res/textures/cracked_earth.jpg")) return;

    GlResourceHolder* resourceHolder = static_cast<GlResourceHolder*>(glfwGetWindowUserPointer(window));
    std::array<uint32_t, 5> textures = resourceHolder->create<Texture2D, 5>();

    auto texture0 = std::make_unique<Texture2D>(textures[0]);
    auto texture1 = std::make_unique<Texture2D>(textures[1]);
    auto texture2 = std::make_unique<Texture2D>(textures[2]);
    auto texture3 = std::make_unique<Texture2D>(textures[3]);
    auto texture4 = std::make_unique<Texture2D>(textures[4]);

    texture0->loadFromImage(imgMask, false, true);
    texture1->loadFromImage(imgSand, true, true);
    texture2->loadFromImage(imgSpace, true, true);
    texture3->loadFromImage(imgStone, true, true);
    texture4->loadFromImage(imgRock, true, true);

    std::array<float, 20> vertices =
    {
        0.0f,  0.0f,   0.0f, 0.0f, 0.0f,
        width, 0.0f,   0.0f, 1.0f, 0.0f,
        width, height, 0.0f, 1.0f, 1.0f,
        0.0f,  height, 0.0f, 0.0f, 1.0f
    };

    std::array<VertexBufferLayout::Attribute, 2> attributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };
    VertexBufferLayout layout(attributes);

    std::array<uint32_t, 2> buffers = resourceHolder->create<GlBuffer, 2>();
    std::array<uint32_t, 1> vertexArrays = resourceHolder->create<VertexArrayObject, 1>();

    GlBuffer vbo(buffers[0], GL_ARRAY_BUFFER);
    vbo.create(sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

    auto vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    vao->addVertexBuffer(vbo, layout);

    std::array<Shader, 2> shaders;
    if(!shaders[0].loadFromFile("res/shaders/dune.vert", GL_VERTEX_SHADER)) return;
    if(!shaders[1].loadFromFile("res/shaders/dune.frag", GL_FRAGMENT_SHADER)) return;

    auto program = std::make_unique<ShaderProgram>();
    if(!program->link(shaders)) return;

    glUseProgram(program->getHandle().value());
    glUniform1i(program->getUniformLocation("texMap").value(), 0);
    glUniform1i(program->getUniformLocation("texSand").value(), 1);
    glUniform1i(program->getUniformLocation("texSpice").value(), 2);
    glUniform1i(program->getUniformLocation("texStone").value(), 3);
    glUniform1i(program->getUniformLocation("texRock").value(), 4);
    glUseProgram(0);

    GlBuffer uniformBuffer(buffers[1], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = std::make_unique<Orthogonal>();
    camera->setupProjectionMatrix(width, height);

    while (!glfwWindowShouldClose(window))
    {
        if(isKeyPressed(GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, true);
            continue;
        }

        if(isKeyPressed(GLFW_KEY_W))
            camera->move(0.0f, 3.0f);

        if(isKeyPressed(GLFW_KEY_A))
            camera->move(3.0f, 0.0f);

        if(isKeyPressed(GLFW_KEY_S))
            camera->move(0.0f, -3.0f);

        if(isKeyPressed(GLFW_KEY_D))
            camera->move(-3.0f, 0.0f);

        uniformBuffer.update(0, sizeof(glm::mat4), 1, static_cast<const void*>(glm::value_ptr(camera->getModelViewProjectionMatrix())));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program->getHandle().value());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0->getHandle());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1->getHandle());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture2->getHandle());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture3->getHandle());
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texture4->getHandle());

        glBindVertexArray(vao->getHandle());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}