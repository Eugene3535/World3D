

template<class T, size_t N>
std::array<GLuint, N> GlResourceHolder::create() noexcept
{
    if constexpr(std::is_same_v<T, GlBuffer>)
    {
        return createResources<N>(m_buffers, glGenBuffers);
    }
    else if constexpr(std::is_same_v<T, VertexArrayObject>)
    {
        return createResources<N>(m_arrays, glGenVertexArrays);
    }
    else if constexpr(std::is_same_v<T, Texture2D>)
    {
        return createResources<N>(m_textures, glGenTextures);
    }
    else
    {
        static_assert(false, "Resource must be implemented!\n");
    }
}


template<class T, size_t N>
void GlResourceHolder::destroy(const std::array<GLuint, N>& objects) noexcept
{
    if constexpr(std::is_same_v<T, GlBuffer>)
    {
        destroyResources<N>(objects, m_buffers, glDeleteBuffers);
    }
    else if constexpr(std::is_same_v<T, VertexArrayObject>)
    {
        destroyResources<N>(objects, m_arrays, glDeleteVertexArrays);
    }
    else if constexpr(std::is_same_v<T, Texture2D>)
    {
        destroyResources<N>(objects, m_textures, glDeleteTextures);
    }
    else
    {
        static_assert(false, "Resource must be implemented!\n");
    }
}


template<size_t N>
std::array<GLuint, N> GlResourceHolder::createResources(std::vector<GLuint>& handles, void(*func)(GLint, GLuint*)) noexcept
{
    std::array<GLuint, N> objects;

    func(static_cast<GLint>(N), objects.data());
    handles.insert(handles.end(), objects.begin(), objects.end());

    return objects;
}


template<size_t N>
void GlResourceHolder::destroyResources(const std::array<GLuint, N>& objects, std::vector<GLuint>& handles, void(*func)(GLint, const GLuint*)) noexcept
{
//  Avoid deleting non-existent objects
    std::vector<GLuint> tmp;
    tmp.reserve(N);

    for(GLuint object : objects)
    {
        for(size_t i = 0; i < handles.size(); ++i)
        {
            if(auto& handle = handles[i]; handle == object)
            {
                tmp.push_back(object);
                std::swap(handle, handles.back());
                handles.pop_back();
                break;
            }
        }
    }
    
    if(!tmp.empty())
        func(static_cast<GLint>(tmp.size()), tmp.data());
}