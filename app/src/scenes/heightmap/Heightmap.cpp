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


void heightmap_demo(GLFWwindow* window) noexcept
{
    glEnable(GL_DEPTH_TEST);

    auto isKeyPressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    AppData* appData = static_cast<AppData*>(glfwGetWindowUserPointer(window));

    std::array<uint32_t, 1> buffer = appData->resourceHolder.create<GlBuffer, 1>();

    GlBuffer uniformBuffer(buffer[0], GL_UNIFORM_BUFFER);
    uniformBuffer.create(sizeof(glm::mat4), 1, nullptr, GL_DYNAMIC_DRAW);
    uniformBuffer.bindBufferRange(0, 0, sizeof(glm::mat4));

    auto camera = &appData->camera.perspective;
    camera->setupProjectionMatrix(45, static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);
    camera->setPosition(30, 3, 30);

    std::vector<float> heightmap;
    uint32_t mapDepth;
    uint32_t mapWidth;

    Image imageMap;        imageMap.loadFromFile("res/textures/heightmap.png");
    Image imgCrackedEarth; imgCrackedEarth.loadFromFile("res/textures/cracked_earth.jpg");
    Image imgRock;         imgRock.loadFromFile("res/textures/rock.jpg");
    Image imgGrass;        imgGrass.loadFromFile("res/textures/grass.jpg");
    Image imgClover;       imgClover.loadFromFile("res/textures/clover.png");

    std::array<uint32_t, 4> textures = appData->resourceHolder.create<Texture2D, 4>();
    
    auto texCrackedEarth = std::make_unique<Texture2D>(textures[0]);
    auto texRock         = std::make_unique<Texture2D>(textures[1]);
    auto texGrass        = std::make_unique<Texture2D>(textures[2]);
    auto texClover       = std::make_unique<Texture2D>(textures[3]);

    texCrackedEarth->loadFromImage(imgCrackedEarth, true, true);
    texRock->loadFromImage(imgRock, true, true);
    texGrass->loadFromImage(imgGrass, true, true);
    texClover->loadFromImage(imgClover, true, true);

    const uint8_t* pixels = imageMap.getPixels();
    mapDepth = imageMap.getHeight();
    mapWidth = imageMap.getWidth();

    heightmap.resize(mapDepth * mapWidth);
    std::vector<float> vertices(mapDepth * mapWidth * 5);
    std::vector<uint32_t> indices;
    indices.reserve(static_cast<size_t>(mapDepth * mapWidth * 2));

    size_t index = 0;
    size_t stride = 0;

    for (size_t z = 0; z < mapDepth; ++z)
    {
        for (size_t x = 0; x < mapWidth; ++x)
        {
            const uint8_t* pixel = pixels + ((z * mapWidth + x) * imageMap.getBytePerPixel());
            int32_t y = static_cast<int32_t>(pixel[0]);
            float Y = y * 0.03f;

            float* vertex = vertices.data() + stride;

            vertex[0] = x;
            vertex[1] = Y;
            vertex[2] = z;
            vertex[3] = x * 0.3f;
            vertex[4] = z * 0.3f;

            heightmap[index] = Y;

            ++index;
            stride += 5;
        }
    }

    for(uint32_t i = 0; i < mapDepth - 1; ++i) // 19 800
    {
        for(uint32_t j = 0; j < mapWidth; ++j)
        {
            indices.push_back(mapWidth * i + j);
            indices.push_back(mapWidth * (i + 1) + j);       
        }
    }

    std::array<uint32_t, 2> buffers = appData->resourceHolder.create<GlBuffer, 2>();
    std::array<uint32_t, 1> vertexArrays = appData->resourceHolder.create<VertexArrayObject, 1>();

    std::array<VertexBufferLayout::Attribute, 2> attributes
    {
        VertexBufferLayout::Attribute::Float3,
        VertexBufferLayout::Attribute::Float2
    };
    VertexBufferLayout layout(attributes);

    GlBuffer vbo(buffers[0], GL_ARRAY_BUFFER);
    GlBuffer ebo(buffers[1], GL_ELEMENT_ARRAY_BUFFER);

//  Just to give you an example, you can pass a pointer to the data right away, or you can fill the buffer later on
    vbo.create(sizeof(float), vertices.size(), nullptr, GL_STATIC_DRAW);
    vbo.update(0, sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()));
    ebo.create(sizeof(uint32_t), indices.size(), static_cast<const void*>(indices.data()), GL_STATIC_DRAW);

    auto vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);
    vao->addVertexBuffer(vbo, layout);
    vao->setElementBuffer(ebo);

    std::array<Shader, 2> shaders;
    shaders[0].loadFromFile("res/shaders/heightmap.vert", GL_VERTEX_SHADER);
    shaders[1].loadFromFile("res/shaders/heightmap.frag", GL_FRAGMENT_SHADER);

    auto program = std::make_unique<ShaderProgram>();
    program->link(shaders);

    glUseProgram(program->getHandle().value());
    glUniform1i(program->getUniformLocation("cracked_earth").value(), 0);
    glUniform1i(program->getUniformLocation("rock").value(), 1);
    glUniform1i(program->getUniformLocation("grass").value(), 2);
    glUniform1i(program->getUniformLocation("clover").value(), 3);
    glUseProgram(0);

    auto getHeightInPoint = [mapWidth, mapDepth, &heightmap](float x, float z) -> float
    {
        if((x >= 0) && (x < mapWidth) && (z >= 0) && (z < mapDepth))
        {
            int32_t cX = static_cast<int32_t>(x);
            int32_t cZ = static_cast<int32_t>(z);

            const float* h = heightmap.data() + (static_cast<int32_t>(z) * mapWidth + static_cast<int32_t>(x));

            float h0 = *h;
            float h1 = *(h + 1);
            float h2 = *(h + mapWidth);
            float h3 = *(h + mapWidth + 1);

            float h5 = ((1 - (x - cX)) * h0 + (x - cX) * h1);
            float h6 = ((1 - (x - cX)) * h2 + (x - cX) * h3);

            return (1 - (z - cZ)) * h5 + (z - cZ) * h6;
        }

        return 0.f;
    };

    while (!glfwWindowShouldClose(window))
    {
        if(isKeyPressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            continue;
        }

        if(isKeyPressed(GLFW_KEY_W))
            camera->moveForward(10);

        if(isKeyPressed(GLFW_KEY_A))
            camera->moveLeft(10);

        if(isKeyPressed(GLFW_KEY_S))
            camera->moveBackward(10);

        if(isKeyPressed(GLFW_KEY_D))
            camera->moveRight(10);

        auto playerPos = camera->getPosition();
        playerPos.y = getHeightInPoint(playerPos.x, playerPos.z) + 1.7f;
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

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program->getHandle().value());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texCrackedEarth->getHandle());
    
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texRock->getHandle());
    
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texGrass->getHandle());
    
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texClover->getHandle());
    
        const uint32_t numStrips = mapWidth - 1;
        const uint32_t numTrisPerStrip = mapWidth * 2 - 2;
    
        glBindVertexArray(vao->getHandle());
    
        for (uint32_t strip = 0; strip < numStrips; ++strip)
            glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip + 2, GL_UNSIGNED_INT, reinterpret_cast<const void*>(sizeof(GLuint) * (numTrisPerStrip + 2) * strip));
    
        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}