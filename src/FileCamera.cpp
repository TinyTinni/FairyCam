#include "FileCamera.hpp"

#include <opencv2/imgcodecs.hpp>

namespace FairyCam
{

bool FileCamera::grab()
{
    if (!m_is_open)
        return false;

    m_current = m_next;
    if (m_current == m_opts.files.cend())
        return false;

    ++m_next;
    if (m_opts.circular && m_next == m_opts.files.cend())
        m_next = m_opts.files.begin();

    return std::filesystem::exists(*m_current);
}

bool FileCamera::retrieve(cv::OutputArray image, int flag)
{
    if (!m_is_open || m_current == m_opts.files.cend())
        return false;
    auto mat = cv::imread(*m_current);
    image.assign(mat);
    return !mat.empty();
}

bool FileCamera::read(cv::OutputArray image)
{
    if (!grab())
    {
        image.assign(cv::Mat());
        return false;
    }
    return retrieve(image);
}

} // namespace FairyCam
