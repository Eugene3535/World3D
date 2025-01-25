

template<class T, size_t N>
std::array<uint32_t, N> GlResourceHolder::create() noexcept
{
    if constexpr(std::is_same_v<T, GlBuffer>)
    {
        return createResources<N>(m_buffers, genBuffers);
    }
    else if constexpr(std::is_same_v<T, VertexArrayObject>)
    {
        return createResources<N>(m_arrays, genVertexArrays);
    }
    else if constexpr(std::is_same_v<T, Texture2D>)
    {
        return createResources<N>(m_textures, genTextures);
    }
    else
    {
        static_assert(false, "Resource must be implemented!\n");
    }
}


template<class T, size_t N>
void GlResourceHolder::destroy(const std::array<uint32_t, N>& objects) noexcept
{
    if constexpr(std::is_same_v<T, GlBuffer>)
    {
        destroyResources<N>(objects, m_buffers, delBuffers);
    }
    else if constexpr(std::is_same_v<T, VertexArrayObject>)
    {
        destroyResources<N>(objects, m_arrays, delVertexArrays);
    }
    else if constexpr(std::is_same_v<T, Texture2D>)
    {
        destroyResources<N>(objects, m_textures, delTextures);
    }
    else
    {
        static_assert(false, "Resource must be implemented!\n");
    }
}


template<size_t N>
std::array<uint32_t, N> GlResourceHolder::createResources(std::vector<uint32_t>& handles, void(*func)(int32_t, uint32_t*)) noexcept
{
    std::array<uint32_t, N> objects;

    if(m_instance && func)
    {
        func(static_cast<int32_t>(N), objects.data());
        handles.insert(handles.end(), objects.begin(), objects.end());
    }

    return objects;
}


template<size_t N>
void GlResourceHolder::destroyResources(const std::array<uint32_t, N>& objects, std::vector<uint32_t>& handles, void(*func)(int32_t, const uint32_t*)) noexcept
{
    if(m_instance && func)
    {
//  Avoid deleting non-existent objects
        std::vector<uint32_t> tmp;
        tmp.reserve(N);

        for(uint32_t object : objects)
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
            func(static_cast<int32_t>(tmp.size()), tmp.data());
    }
}