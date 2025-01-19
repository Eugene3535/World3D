#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window/RenderWindow.hpp"
#include "Heightmap.hpp"


Heightmap::Heightmap(void* handle) noexcept:
    Scene(handle),
    m_rw(handle),
    m_mapWidth(0),
    m_mapDepth(0)
{
    m_imageMap.loadFromFile("res/textures/heightmap.png");
    m_imgCrackedEarth.loadFromFile("res/textures/cracked_earth.jpg");
    m_imgRock.loadFromFile("res/textures/rock.jpg");
    m_imgGrass.loadFromFile("res/textures/grass.jpg");
    m_imgClover.loadFromFile("res/textures/clover.png");

    std::array<uint32_t, 4> textures = m_bufferHolder.create<Texture2D, 4>();

    m_texCrackedEarth = std::make_unique<Texture2D>(textures[0]);
    m_texRock         = std::make_unique<Texture2D>(textures[1]);
    m_texGrass        = std::make_unique<Texture2D>(textures[2]);
    m_texClover       = std::make_unique<Texture2D>(textures[3]);

    m_texCrackedEarth->loadFromImage(m_imgCrackedEarth, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear);
    m_texRock->loadFromImage(m_imgRock, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear);
    m_texGrass->loadFromImage(m_imgGrass, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear);
    m_texClover->loadFromImage(m_imgClover, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear);

    const uint8_t* pixels = m_imageMap.getPixels();
    m_mapDepth = m_imageMap.getHeight();
    m_mapWidth = m_imageMap.getWidth();

    m_heightmap.resize(m_mapDepth * m_mapWidth);
    std::vector<float> vertices(m_mapDepth * m_mapWidth * 5);
    std::vector<uint32_t> indices;
    indices.reserve(static_cast<size_t>(m_mapDepth * m_mapWidth * 2));

    size_t index = 0;
    size_t stride = 0;

    for (size_t z = 0; z < m_mapDepth; ++z)
    {
        for (size_t x = 0; x < m_mapWidth; ++x)
        {
            const uint8_t* pixel = pixels + ((z * m_mapWidth + x) * m_imageMap.getBytePerPixel());
            int32_t y = static_cast<int32_t>(pixel[0]);
            float Y = y * 0.03f;

            float* vertex = vertices.data() + stride;

            vertex[0] = x;
            vertex[1] = Y;
            vertex[2] = z;
            vertex[3] = x * 0.3f;
            vertex[4] = z * 0.3f;

            m_heightmap[index] = Y;

            ++index;
            stride += 5;
        }
    }

    for(uint32_t i = 0; i < m_mapDepth - 1; ++i) // 19 800
    {
        for(uint32_t j = 0; j < m_mapWidth; ++j)
        {
            indices.push_back(m_mapWidth * i + j);
            indices.push_back(m_mapWidth * (i + 1) + j);       
        }
    }

    std::array<uint32_t, 3> buffers = m_bufferHolder.create<GlBuffer, 3>();
    std::array<uint32_t, 1> vertexArrays = m_bufferHolder.create<VertexArray, 1>();

    VertexBufferLayout layout
    {
        AttributeInfo::Float3,
        AttributeInfo::Float2
    };

    m_vbo = std::make_unique<VertexBuffer>(buffers[0], layout);
    m_ebo = std::make_unique<IndexBuffer>(buffers[1]);

//  Just to give you an example, you can pass a pointer to the data right away, or you can fill the buffer later on
    m_vbo->create(nullptr, vertices.size(), sizeof(float), GlBuffer::Usage::Static);
    m_vbo->update(vertices.data(), vertices.size(), sizeof(float), 0);
    m_ebo->create(indices.data(), indices.size(), sizeof(uint32_t), GlBuffer::Usage::Static);

    m_vao = std::make_unique<VertexArray>(vertexArrays[0]);

    m_vao->addVertexBuffer(*m_vbo);
    m_vao->setIndexBuffer(*m_ebo);

    std::initializer_list<Shader> shaders = 
    {
        { "res/shaders/heightmap.vert", Shader::Vertex   }, 
        { "res/shaders/heightmap.frag", Shader::Fragment } 
    };

    m_program = std::make_unique<ShaderProgram>(shaders);
    ShaderProgram::bind(m_program.get());
    ShaderProgram::setUniform1i(m_program->getUniformLocation("cracked_earth"), 0);
    ShaderProgram::setUniform1i(m_program->getUniformLocation("rock"), 1);
    ShaderProgram::setUniform1i(m_program->getUniformLocation("grass"), 2);
    ShaderProgram::setUniform1i(m_program->getUniformLocation("clover"), 3);

    auto rwnd = (RenderWindow*)handle;
    auto size = rwnd->getSize();
    m_perspective = std::make_unique<Perspective>(buffers[2]);
    m_perspective->setupProjectionMatrix(45, static_cast<float>(size.x) / static_cast<float>(size.y), 0.1f, 1000.0f);
}


Heightmap::~Heightmap() noexcept
{

}


void Heightmap::draw() noexcept
{
    auto get_height_in_point = [this](float x, float z) -> float
    {
        if((x >= 0) && (x < m_mapWidth) && (z >= 0) && (z < m_mapDepth))
        {
            int32_t cX = static_cast<int32_t>(x);
            int32_t cZ = static_cast<int32_t>(z);

            float* h = m_heightmap.data() + (static_cast<int32_t>(z) * m_mapWidth + static_cast<int32_t>(x));

            float h0 = *h;
            float h1 = *(h + 1);
            float h2 = *(h + m_mapWidth);
            float h3 = *(h + m_mapWidth + 1);

            float h5 = ((1 - (x - cX)) * h0 + (x - cX) * h1);
            float h6 = ((1 - (x - cX)) * h2 + (x - cX) * h3);

            return (1 - (z - cZ)) * h5 + (z - cZ) * h6;
        }

        return 0.f;
    };

    auto rwnd = (RenderWindow*)m_rw;

    auto cur = rwnd->getCursorPosition();
    auto pos = rwnd->getPosition();
    auto siz = rwnd->getSize();

    pos.x += siz.x >> 1;
    pos.y += siz.y >> 1;

    m_perspective->rotateX((pos.x - cur.x) * 0.125f);
    m_perspective->rotateY((pos.y - cur.y) * 0.125f);

    rwnd->setCursorPosition(pos.x, pos.y);

    m_perspective->setPosition(30, 3, 30);
    m_perspective->apply(0.01f);

    Texture2D::enable(0);
    Texture2D::bind(m_texCrackedEarth.get());

    Texture2D::enable(1);
    Texture2D::bind(m_texRock.get());

    Texture2D::enable(2);
    Texture2D::bind(m_texGrass.get());

    Texture2D::enable(3);
    Texture2D::bind(m_texClover.get());

    m_vao->bind(m_vao.get());

    const uint32_t numStrips = m_mapWidth - 1;
    const uint32_t numTrisPerStrip = m_mapWidth * 2 - 2;

    drawHeightmap(numStrips, numTrisPerStrip);

    m_vao->bind(nullptr);
}