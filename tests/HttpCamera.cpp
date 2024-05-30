#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <doctest.h>
#include <fstream>

#include <opencv2/core.hpp>

import FairyCam;

using namespace FairyCam;

uint getPort()
{
    static uint port = 5000;
    return port++;
}

bool sendFileImage(Poco::Net::HTTPClientSession &client, std::string_view path)
{
    std::ifstream file;
    file.open(std::string{path}, std::ios::binary);
    if (!file.is_open())
        return false;

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, "/image",
                                   Poco::Net::HTTPMessage::HTTP_1_1);
    auto &ostream = client.sendRequest(request);
    Poco::StreamCopier::copyStream(file, ostream);

    Poco::Net::HTTPResponse response;
    client.receiveResponse(response);
    bool result = response.getStatus() ==
                  Poco::Net::HTTPResponse::HTTPStatus::HTTP_ACCEPTED;
    return result;
}

bool sendInvalidImage(Poco::Net::HTTPClientSession &client)
{

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, "/image",
                                   Poco::Net::HTTPMessage::HTTP_1_1);
    auto &ostream = client.sendRequest(request);
    ostream << "invalid image string";

    Poco::Net::HTTPResponse response;
    client.receiveResponse(response);
    bool result = response.getStatus() ==
                  Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
    return result;
}

TEST_SUITE("HttpCamera")
{
    TEST_CASE("close and reopen")
    {
        AnyCamera cam(AnyCamera::create<HttpCamera>(
            {.port = getPort(), .grabTimeOutMs = 0}));

        REQUIRE(cam.open(0, 0, {}));
        REQUIRE(cam.isOpened());
        cam.release();
        REQUIRE_FALSE(cam.isOpened());
        cv::Mat m;
        REQUIRE_FALSE(cam.read(m));
        REQUIRE(m.empty());
        REQUIRE(cam.open(0, 0, {}));
        REQUIRE(cam.isOpened());
        cam.release();
        REQUIRE_FALSE(cam.isOpened());
    }

    TEST_CASE("release queue")
    {
        const auto port = getPort();
        AnyCamera cam(
            AnyCamera::create<HttpCamera>({.port = port, .grabTimeOutMs = 0}));
        Poco::Net::HTTPClientSession client("127.0.0.1", port);

        REQUIRE(cam.open(0, 0, {}));
        REQUIRE(cam.isOpened());

        REQUIRE(sendFileImage(client, TEST_DATA "test.png"));

        cam.release();
        REQUIRE(cam.open(0, 0, {}));
        REQUIRE(cam.isOpened());
        cv::Mat m;
        REQUIRE(!cam.read(m));
        CHECK(m.empty());
    }

    TEST_CASE("transfer images in queue")
    {
        for (auto n_images : std::vector{1, 10, 100})
        {
            CAPTURE(n_images);
            const auto port = getPort();
            AnyCamera cam(AnyCamera::create<HttpCamera>(
                {.port = port, .grabTimeOutMs = 0}));
            Poco::Net::HTTPClientSession client("127.0.0.1", port);

            REQUIRE(cam.open(0, 0, {}));
            REQUIRE(cam.isOpened());

            for (int i = 0; i < n_images; ++i)
            {
                REQUIRE(sendFileImage(client, TEST_DATA "test.png"));
            }

            for (int i = 0; i < n_images; ++i)
            {
                cv::Mat m;
                REQUIRE(cam.read(m));
                CHECK(!m.empty());
            }
            cv::Mat m;
            REQUIRE(!cam.read(m));
            CHECK(m.empty());
        }
    }
    TEST_CASE("transfer multiple images out of queue")
    {
        for (auto n_images : std::vector{10})
        {
            CAPTURE(n_images);
            const auto port = getPort();
            AnyCamera cam(AnyCamera::create<HttpCamera>(
                {.port = port, .grabTimeOutMs = 0}));
            Poco::Net::HTTPClientSession client("127.0.0.1", port);

            REQUIRE(cam.open(0, 0, {}));
            REQUIRE(cam.isOpened());

            cv::Mat m;
            for (auto i = 0; i < n_images; ++i)
            {
                REQUIRE(sendFileImage(client, TEST_DATA "test.png"));
                REQUIRE(cam.read(m));
                CHECK(!m.empty());
            }
            REQUIRE(!cam.read(m));
            CHECK(m.empty());
        }
    }

    TEST_CASE("clear queue by HTTP DELETE")
    {
        for (auto n_images : std::vector{10})
        {
            CAPTURE(n_images);
            const auto port = getPort();
            AnyCamera cam(AnyCamera::create<HttpCamera>(
                {.port = port, .grabTimeOutMs = 0}));
            Poco::Net::HTTPClientSession client("127.0.0.1", port);

            REQUIRE(cam.open(0, 0, {}));
            REQUIRE(cam.isOpened());

            for (auto i = 0; i < n_images; ++i)
            {
                REQUIRE(sendFileImage(client, TEST_DATA "test.png"));
            }

            Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_DELETE,
                                           "/image",
                                           Poco::Net::HTTPMessage::HTTP_1_1);
            client.sendRequest(request);
            Poco::Net::HTTPResponse response;
            client.receiveResponse(response);
            CHECK(response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK);

            // no images are left in the queue
            cv::Mat m;
            REQUIRE(!cam.read(m));
            CHECK(m.empty());
        }
    }

    TEST_CASE("transfer multiple valid and invalid images out of queue")
    {
        for (auto n_images : std::vector{10})
        {
            CAPTURE(n_images);
            const auto port = getPort();
            AnyCamera cam(AnyCamera::create<HttpCamera>(
                {.port = port, .grabTimeOutMs = 0}));
            Poco::Net::HTTPClientSession client("127.0.0.1", port);

            REQUIRE(cam.open(0, 0, {}));
            REQUIRE(cam.isOpened());

            cv::Mat m;
            for (auto i = 0; i < n_images; ++i)
            {
                CAPTURE(i);
                if (i % 2)
                {
                    REQUIRE(sendFileImage(client, TEST_DATA "test.png"));
                    REQUIRE(cam.read(m));
                    CHECK(!m.empty());
                }
                else
                {
                    REQUIRE(sendInvalidImage(client));
                    REQUIRE(!cam.read(m));
                    CHECK(m.empty());
                }
            }
            REQUIRE(!cam.read(m));
            CHECK(m.empty());
        }
    }
}
