#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl/context/GlContext.hpp"
#include "window/RenderWindow.hpp"
#include "Heightmap.hpp"


Heightmap::Heightmap() noexcept:
    Scene(),
    m_mapWidth(0),
    m_mapDepth(0)
{
    Image imageMap;        imageMap.loadFromFile("res/textures/heightmap.png");
    Image imgCrackedEarth; imgCrackedEarth.loadFromFile("res/textures/cracked_earth.jpg");
    Image imgRock;         imgRock.loadFromFile("res/textures/rock.jpg");
    Image imgGrass;        imgGrass.loadFromFile("res/textures/grass.jpg");
    Image imgClover;       imgClover.loadFromFile("res/textures/clover.png");

    std::array<uint32_t, 4> textures = Context->create<Texture2D, 4>();

    m_texCrackedEarth = std::make_unique<Texture2D>(textures[0]);
    m_texRock         = std::make_unique<Texture2D>(textures[1]);
    m_texGrass        = std::make_unique<Texture2D>(textures[2]);
    m_texClover       = std::make_unique<Texture2D>(textures[3]);

    m_texCrackedEarth->loadFromImage(imgCrackedEarth, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear);
    m_texRock->loadFromImage(imgRock, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear);
    m_texGrass->loadFromImage(imgGrass, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear);
    m_texClover->loadFromImage(imgClover, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear);

    const uint8_t* pixels = imageMap.getPixels();
    m_mapDepth = imageMap.getHeight();
    m_mapWidth = imageMap.getWidth();

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
            const uint8_t* pixel = pixels + ((z * m_mapWidth + x) * imageMap.getBytePerPixel());
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

    std::array<uint32_t, 2> buffers = Context->create<GlBuffer, 2>();
    std::array<uint32_t, 1> vertexArrays = Context->create<VertexArrayObject, 1>();

    std::array<AttributeInfo, 2> attributes
    {
        AttributeInfo::Float3,
        AttributeInfo::Float2
    };
    VertexBufferLayout layout(attributes);

    m_vbo = std::make_unique<VertexBuffer>(buffers[0], layout);
    m_ebo = std::make_unique<IndexBuffer>(buffers[1]);

//  Just to give you an example, you can pass a pointer to the data right away, or you can fill the buffer later on
    m_vbo->create(sizeof(float), vertices.size(), nullptr, GlBuffer::Usage::Static);
    m_vbo->update(0, sizeof(float), vertices.size(), static_cast<const void*>(vertices.data()));
    m_ebo->create(sizeof(uint32_t), indices.size(), static_cast<const void*>(indices.data()), GlBuffer::Usage::Static);

    m_vao = std::make_unique<VertexArrayObject>(vertexArrays[0]);

    m_vao->addVertexBuffer(*m_vbo);
    m_vao->setIndexBuffer(*m_ebo);

    std::array<Shader, 2> shaders;
    shaders[0].loadFromFile("res/shaders/heightmap.vert", Shader::Vertex);
    shaders[1].loadFromFile("res/shaders/heightmap.frag", Shader::Fragment);

    m_program = std::make_unique<ShaderProgram>();
    m_program->link(shaders);

    ShaderProgram::bind(m_program.get());
    ShaderProgram::setUniform1i(m_program->getUniformLocation("cracked_earth"), 0);
    ShaderProgram::setUniform1i(m_program->getUniformLocation("rock"), 1);
    ShaderProgram::setUniform1i(m_program->getUniformLocation("grass"), 2);
    ShaderProgram::setUniform1i(m_program->getUniformLocation("clover"), 3);
    ShaderProgram::bind(nullptr);
}


Heightmap::~Heightmap() noexcept
{

}


void Heightmap::draw() noexcept
{
    ShaderProgram::bind(m_program.get());

    Texture2D::enable(0);
    Texture2D::bind(m_texCrackedEarth.get());

    Texture2D::enable(1);
    Texture2D::bind(m_texRock.get());

    Texture2D::enable(2);
    Texture2D::bind(m_texGrass.get());

    Texture2D::enable(3);
    Texture2D::bind(m_texClover.get());

    const uint32_t numStrips = m_mapWidth - 1;
    const uint32_t numTrisPerStrip = m_mapWidth * 2 - 2;

    m_vao->bind(m_vao.get());

    for (uint32_t strip = 0; strip < numStrips; ++strip)
        m_vao->drawElements(PrimitiveType::TriangleStrip, numTrisPerStrip + 2, reinterpret_cast<const void*>(sizeof(uint32_t) * (numTrisPerStrip + 2) * strip));

    m_vao->bind(nullptr);

    ShaderProgram::bind(nullptr);
}


float Heightmap::getHeightInPoint(float x, float z) const noexcept
{
    if((x >= 0) && (x < m_mapWidth) && (z >= 0) && (z < m_mapDepth))
    {
        int32_t cX = static_cast<int32_t>(x);
        int32_t cZ = static_cast<int32_t>(z);

        const float* h = m_heightmap.data() + (static_cast<int32_t>(z) * m_mapWidth + static_cast<int32_t>(x));

        float h0 = *h;
        float h1 = *(h + 1);
        float h2 = *(h + m_mapWidth);
        float h3 = *(h + m_mapWidth + 1);

        float h5 = ((1 - (x - cX)) * h0 + (x - cX) * h1);
        float h6 = ((1 - (x - cX)) * h2 + (x - cX) * h3);

        return (1 - (z - cZ)) * h5 + (z - cZ) * h6;
    }

    return 0.f;
}