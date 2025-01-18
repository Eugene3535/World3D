template<size_t N>
void GlBuffer::deleteBuffers(const std::array<uint32_t, N>& buffers) noexcept
{
    if(m_instance && m_instance->delBuffers)
    {
        std::vector<uint32_t> tmp;
        tmp.reserve(N);

        for(uint32_t buffer : buffers)
        {
            for(size_t i = 0; i < m_instance->m_buffers.size(); ++i)
            {
                if(auto& handle = m_instance->m_buffers[i]; handle == buffer)
                {
                    tmp.push_back(buffer);
                    std::swap(handle, m_instance->m_buffers.back());
                    m_instance->m_buffers.pop_back();
                    break;
                }
            }
        }

        m_instance->delBuffers(static_cast<int32_t>(tmp.size()), tmp.data());
    }
}


template<size_t N>
std::array<uint32_t, N> GlBuffer::generateBuffers() noexcept
{
    std::array<uint32_t, N> buffers;

    if(m_instance && m_instance->genBuffers)
    {
        m_instance->genBuffers(static_cast<int32_t>(N), buffers.data());
        m_instance->m_buffers.insert(m_instance->m_buffers.end(), buffers.begin(), buffers.end());
    }

    return buffers;
}