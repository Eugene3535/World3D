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
#include "Texture.hpp"
#include "Vertex.hpp"
#include "ShaderProgram.hpp"

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

    GLuint texSnow     = Texture().createFromImage(imgSnow, GL_REPEAT, GL_LINEAR);
    GLuint texPavement = Texture().createFromImage(imgPavement, GL_REPEAT, GL_LINEAR);
    GLuint texPath     = Texture().createFromImage(imgPath, GL_CLAMP_TO_BORDER, GL_LINEAR);

    float mapWidth = static_cast<float>(imgPath.getWidth());
    float mapHeight = static_cast<float>(imgPath.getHeight());

    std::array<Vertex, 4> vertices = 
    {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f),          glm::vec2(0.0f,  0.0f)),
        Vertex(glm::vec3(mapWidth, 0.0f, 0.0f),      glm::vec2(10.0f, 0.0f)),
        Vertex(glm::vec3(mapWidth, 0.0f, mapHeight), glm::vec2(10.0f, 10.0f)),
        Vertex(glm::vec3(0.0f, 0.0f, mapHeight),     glm::vec2(0.0f,  10.0f))
    };

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, uv)));
    glEnableVertexAttribArray(1);
   
    glBindVertexArray(0);

    GLuint shader = ShaderProgram().compile("res/shaders/ground.vert", "res/shaders/ground.frag");
    glUseProgram(shader);
    int mvpLoc = glGetUniformLocation(shader, "MVP");

    glUniform1i(glGetUniformLocation(shader, "snowSampler"), 0);
    glUniform1i(glGetUniformLocation(shader, "pavementSampler"), 1);
    glUniform1i(glGetUniformLocation(shader, "pathSampler"), 2);

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
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texSnow);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texPavement);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texPath);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glDeleteTextures(1, &texPath);
    glDeleteTextures(1, &texPavement);  
    glDeleteTextures(1, &texPath); 
}