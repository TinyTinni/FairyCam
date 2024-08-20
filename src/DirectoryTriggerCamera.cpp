#include "DirectoryTriggerCamera.hpp"

#include <Poco/Delegate.h>
#include <Poco/DirectoryWatcher.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>
#include <opencv2/imgcodecs.hpp>

namespace FairyCam
{

struct DirectoryTriggerCamera::Impl
{
    class NewImageNotification : public Poco::Notification
    {
        std::filesystem::path m_path;

      public:
        NewImageNotification(std::filesystem::path p) : m_path{std::move(p)} {}
        virtual ~NewImageNotification() override = default;
        const std::filesystem::path &path() const noexcept { return m_path; }
    };

    void onItemAdded(const Poco::DirectoryWatcher::DirectoryEvent &event)
    {
        m_queue.enqueueNotification(
            new NewImageNotification(event.item.path()));
    }

    Options m_opts;
    Poco::NotificationQueue m_queue;
    std::unique_ptr<Poco::DirectoryWatcher> m_watcher = nullptr;
    Poco::AutoPtr<NewImageNotification> m_msg = nullptr;

    Impl(Options opts) : m_opts{std::move(opts)} {}
};

DirectoryTriggerCamera::DirectoryTriggerCamera(Options opts)
    : d{std::make_unique<Impl>(std::move(opts))}
{
}

DirectoryTriggerCamera::~DirectoryTriggerCamera() { release(); }

bool DirectoryTriggerCamera::open(
    [[maybe_unused]] int idx, [[maybe_unused]] int apiPreference,
    [[maybe_unused]] const std::vector<int> &params)
{
    if (!std::filesystem::is_directory(d->m_opts.dir))
        return false;

    d->m_watcher = std::make_unique<Poco::DirectoryWatcher>(
        d->m_opts.dir.string(), Poco::DirectoryWatcher::DW_ITEM_ADDED);

    d->m_watcher->itemAdded +=
        Poco::delegate(d.get(), &DirectoryTriggerCamera::Impl::onItemAdded);

    return true;
}

bool DirectoryTriggerCamera::isOpened() const
{
    return d->m_watcher != nullptr;
}

void DirectoryTriggerCamera::release()
{
    d->m_watcher.reset();
    d->m_msg.reset();
    d->m_queue.clear();
}

bool DirectoryTriggerCamera::grab()
{
    if (!isOpened())
        return false;

    d->m_msg = dynamic_cast<Impl::NewImageNotification *>(
        d->m_queue.waitDequeueNotification(d->m_opts.grabTimeOutMs));
    return d->m_msg != nullptr;
}

bool DirectoryTriggerCamera::retrieve(cv::OutputArray image,
                                      [[maybe_unused]] int flag)
{
    if (!isOpened())
        return false;
    if (d->m_msg == nullptr)
        return false;

    auto mat = cv::imread(d->m_msg->path().string());
    image.assign(mat);
    return !mat.empty();
}

bool DirectoryTriggerCamera::read(cv::OutputArray image)
{
    if (!grab())
    {
        image.assign(cv::Mat());
        return false;
    }
    return retrieve(image);
}

} // namespace FairyCam
