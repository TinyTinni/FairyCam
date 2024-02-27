#include "HttpCamera.hpp"

#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>

using namespace Poco::Net;

namespace FairyCam
{

struct HttpCamera::Impl
{
    class NewImageNotification : public Poco::Notification
    {
        cv::Mat m_mat;

      public:
        NewImageNotification(cv::Mat m) : m_mat{std::move(m)} {}
        virtual ~NewImageNotification() override = default;
        const cv::Mat &image() const noexcept { return m_mat; }
    };

    class ImageRequestHandler : public HTTPRequestHandler
    {
        Poco::NotificationQueue &m_queue;

      public:
        ImageRequestHandler(Poco::NotificationQueue &queue) : m_queue{queue} {}

        void setImage(HTTPServerRequest &request, HTTPServerResponse &response)
        {
            std::vector<char> buffer(
                std::istreambuf_iterator<char>(request.stream()), {});
            auto notify = Poco::makeAuto<NewImageNotification>(
                cv::imdecode(buffer, cv::IMREAD_UNCHANGED));

            if (notify->image().empty())
            {
                response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
            }
            else
            {
                m_queue.enqueueNotification(std::move(notify));
                response.setStatusAndReason(HTTPResponse::HTTP_ACCEPTED);
            }
        }

        void handleRequest(HTTPServerRequest &request,
                           HTTPServerResponse &response) override
        {
            auto method = request.getMethod();
            if (method == HTTPServerRequest::HTTP_POST)
            {
                setImage(request, response);
            }
            else if (method == HTTPServerRequest::HTTP_DELETE)
            {
                m_queue.clear();
                response.setStatusAndReason(HTTPResponse::HTTP_OK);
            }
            else
            {
                response.setStatusAndReason(HTTPResponse::HTTP_NOT_FOUND);
            }
            response.send();
        }
    };

    class ImageRequestHandlerFactory : public HTTPRequestHandlerFactory
    {
        Poco::NotificationQueue &m_queue;

      public:
        ImageRequestHandlerFactory(Poco::NotificationQueue &queue)
            : m_queue{queue}
        {
        }

        HTTPRequestHandler *
        createRequestHandler(const HTTPServerRequest &request)
        {
            if (request.getURI() == "/image")
                return new ImageRequestHandler(m_queue);
            else
                return 0;
        }
    };

    bool m_started = false;
    Poco::NotificationQueue m_queue;
    Poco::Net::HTTPServer m_srv;
    Poco::AutoPtr<NewImageNotification> m_msg = nullptr;
    HttpCamera::Options m_opts;
    Impl(HttpCamera::Options opts)
        : m_queue{},
          m_srv(Poco::makeShared<ImageRequestHandlerFactory>(m_queue),
                ServerSocket(opts.port), createParams()),
          m_opts{std::move(opts)}
    {
    }

  private:
    static Poco::AutoPtr<HTTPServerParams> createParams()
    {
        auto ptr = Poco::makeAuto<HTTPServerParams>();
        return ptr;
    }
};

HttpCamera::HttpCamera(Options opts) : d(std::make_unique<Impl>(opts)) {}

HttpCamera::~HttpCamera() { release(); }

bool HttpCamera::open(int idx, int apiPreference,
                      const std::vector<int> &params)
{
    d->m_srv.start();
    d->m_started = true;
    return true;
}

bool HttpCamera::isOpened() const noexcept { return d->m_started; }

void HttpCamera::release() noexcept
{

    d->m_started = false;
    d->m_srv.stop();
    d->m_msg.reset();
    d->m_queue.clear();
}

bool HttpCamera::grab()
{
    if (!d->m_started)
        return false;

    d->m_msg = dynamic_cast<Impl::NewImageNotification *>(
        d->m_queue.waitDequeueNotification(d->m_opts.grabTimeOutMs));
    return d->m_msg != nullptr;
}

bool HttpCamera::retrieve(cv::OutputArray image, int flag)
{
    if (d->m_msg == nullptr)
        return false;

    cv::Mat m = d->m_msg->image();
    image.assign(m);
    return !m.empty();
}

bool HttpCamera::read(cv::OutputArray image)
{
    if (!grab())
    {
        image.assign(cv::Mat());
        return false;
    }
    return retrieve(image);
}

} // namespace FairyCam
