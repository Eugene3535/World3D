#include <array>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.H>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Image.hpp"
#include "Texture2D.hpp"
#include "ShaderProgram.hpp"
#include "VertexArray.hpp"

static float lastX = 1200 / 2.0f;
static float lastY = 800 / 2.0f;
static float pitch = 20;
static float yaw = 180;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   -= xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
    pitch =  89.0f;
    if(pitch < -89.0f)
    pitch = -89.0f;
}

void path_demo(GLFWwindow* window, GLint scr_width, GLint scr_height)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    auto is_key_pressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    glEnable(GL_DEPTH_TEST);

    Image imgSnow;     imgSnow.loadFromFile("res/textures/snow.png");
    Image imgPavement; imgPavement.loadFromFile("res/textures/pavement.jpg");
    Image imgPath;     imgPath.loadFromFile("res/textures/test.png");

    Texture2D texSnow     = { imgSnow,     Texture2D::WrapMode::Repeat,        Texture2D::FilterMode::Linear };
    Texture2D texPavement = { imgPavement, Texture2D::WrapMode::Repeat,        Texture2D::FilterMode::Linear };
    Texture2D texPath     = { imgPath,     Texture2D::WrapMode::ClampToBorder, Texture2D::FilterMode::Linear };

    float mapWidth  = static_cast<float>(imgPath.getWidth());
    float mapHeight = static_cast<float>(imgPath.getHeight());

    std::array<float, 20> vertices = 
    {
        0.0f,     0.0f, 0.0f,      0.0f,  0.0f,
        mapWidth, 0.0f, 0.0f,      10.0f, 0.0f,
        mapWidth, 0.0f, mapHeight, 10.0f, 10.0f,
        0.0f,     0.0f, mapHeight, 0.0f,  10.0f
    };

    BufferLayout layout
    {
        AttributeInfo::Float3,
        AttributeInfo::Float2
    };

    auto vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(GLfloat), layout);
    auto vao = std::make_unique<VertexArray>();

    vao->addVertexBuffer(*vbo);

    ShaderProgram program = 
    { 
        { "res/shaders/ground.vert", Shader::Vertex   }, 
        { "res/shaders/ground.frag", Shader::Fragment } 
    };

    if (!program.isCompiled())
        return;

    ShaderProgram::bind(&program);
    int32_t mvpLoc = program.getUniformLocation("MVP");

    ShaderProgram::setUniform1i(program.getUniformLocation("snowSampler"), 0);
    ShaderProgram::setUniform1i(program.getUniformLocation("pavementSampler"), 1);
    ShaderProgram::setUniform1i(program.getUniformLocation("pathSampler"), 2);

    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)scr_width / (float)scr_height, 0.1f, 1000.0f);

    glm::vec2 pos = { 3, 5 };

    while (!glfwWindowShouldClose(window))
    {
        if(is_key_pressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, true);
            continue;
        }

        float radians = glm::radians(yaw);
        float speed = 0.0f;

        if (is_key_pressed(GLFW_KEY_W))   speed = -1.1f;
        if (is_key_pressed(GLFW_KEY_S))   speed =  1.1f;
        if (is_key_pressed(GLFW_KEY_A)) { speed =  1.1f; radians -= M_PI_2; }
        if (is_key_pressed(GLFW_KEY_D)) { speed =  1.1f; radians += M_PI_2; }

        if (speed != 0.0f)
        {
            pos.x += sinf(radians) * speed;
            pos.y += cosf(radians) * speed;
        }

        glm::mat4 model_view = glm::mat4(1.0f);
        model_view = glm::rotate(model_view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model_view = glm::rotate(model_view, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        model_view = glm::translate(model_view, glm::vec3(-pos.x, -100.0f, -pos.y));

        auto MVP = projection * model_view;
        ShaderProgram::setUniformMatrix4fv(mvpLoc, 1, 0, glm::value_ptr(MVP));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        Texture2D::enable(GL_TEXTURE0);
        Texture2D::bind(&texSnow);

        Texture2D::enable(GL_TEXTURE1);
        Texture2D::bind(&texPavement);

        Texture2D::enable(GL_TEXTURE2);
        Texture2D::bind(&texPath);

        vao->bind(vao.get());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        vao->bind(nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}