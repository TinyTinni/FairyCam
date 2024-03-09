#include "MemoryCamera.hpp"

#include <opencv2/imgcodecs.hpp>

namespace FairyCam
{

bool MemoryCamera::grab()
{
    if (!m_is_open)
        return false;

    m_current = m_next;
    if (m_current == m_opts.images.cend())
        return false;

    ++m_next;
    if (m_opts.circular && m_next == m_opts.images.cend())
        m_next = m_opts.images.begin();

    return !m_current->empty();
}

bool MemoryCamera::retrieve(cv::OutputArray image, int flag)
{
    if (!m_is_open || m_current == m_opts.images.cend())
        return false;
    cv::Mat mat = *m_current;
    image.assign(mat);
    return !mat.empty();
}

bool MemoryCamera::read(cv::OutputArray image)
{
    if (!grab())
    {
        image.assign(cv::Mat());
        return false;
    }
    return retrieve(image);
}

} // namespace FairyCam
