#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "view/Camera.hpp"
#include "graphics/Shader.hpp"
#include "loaders/MeshLoader.hpp"

const unsigned SCR_WIDTH = 1200;
const unsigned SCR_HEIGHT = 800;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow* window;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float xoffset;
float yoffset;
bool camUpdate = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
bool IsKeyPressed(int key);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "World3D", nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

//  Callbacks
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGL())
    {
        glfwTerminate();

        return -1;
    }

    glEnable(GL_DEPTH_TEST);

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);

    glDebugMessageCallback( [](GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
        {
            fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                    ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
        }, 0 );
#endif

//  Shaders
    Shader grid_shader;
    {
        if(!grid_shader.compile("res/shaders/shader.vert", "res/shaders/shader.frag"))
        {
            glfwTerminate();

            return -1;
        }
    }
    grid_shader.bind(&grid_shader);

    Shader model_shader;
    {
        if(!model_shader.compile("res/shaders/object_shader.vert", "res/shaders/object_shader.frag"))
        {
            glfwTerminate();

            return -1;
        }
    }

    int grid_mvp = grid_shader.getUniformLocation("MVP");
    int model_mvp = model_shader.getUniformLocation("MVP");

    if((grid_mvp == -1) || (model_mvp == -1))
    {
        glfwTerminate();

        return -1;
    }

    std::vector<VertexBuffer> meshes;

    if(!MeshLoader().load_meshes_from_file("res/models/House_Models.obj", meshes))
    {
        glfwTerminate();

        return -1;
    }

    glm::mat4 model_transform = 
    glm::translate(glm::identity<glm::mat4>(), glm::vec3(20, 1, 20)) * 
    glm::rotate(glm::identity<glm::mat4>(), glm::radians(45.0f), glm::vec3(0, 1, 0)) *
    glm::scale(glm::identity<glm::mat4>(), glm::vec3(2, 2, 2));

    Camera camera;
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);

//  Main loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if(IsKeyPressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, true);
            continue;
        }

        if (IsKeyPressed(GLFW_KEY_W))
            camera.move(Camera::FORWARD, deltaTime);

        if (IsKeyPressed(GLFW_KEY_S))
            camera.move(Camera::BACKWARD, deltaTime);

        if (IsKeyPressed(GLFW_KEY_A))
            camera.move(Camera::LEFT, deltaTime);

        if (IsKeyPressed(GLFW_KEY_D))
            camera.move(Camera::RIGHT, deltaTime);


        if (camUpdate)
        {
            camera.rotate(xoffset * 0.1f, yoffset * 0.1f);
            camUpdate = false;
        }

        glm::mat4 view_projection_matrix = projection * camera.getViewMatrix();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//      MODEL
        Shader::bind(&model_shader);
        glUniformMatrix4fv(model_mvp, 1, GL_FALSE, glm::value_ptr(view_projection_matrix * model_transform)); 

        for(auto& m : meshes)
            m.draw();

        Shader::bind(nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camUpdate = true;
    xoffset = xpos - lastX;
    yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;
}

bool IsKeyPressed(int key)
{
    if( ! window) 
        return false; // Not initialized

    return glfwGetKey(window, key) == GLFW_PRESS;
}