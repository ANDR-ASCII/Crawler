#include <iostream>
#include "../HttpLib/HttpResponse.h"
#include "ConsoleCrawlerViewer.h"

void ViewHandler::ConsoleCrawlerViewer::viewInfo(const std::string &url,
                                          size_type extl,
                                          size_type intl,
                                          size_type indexed) const
{
    std::cout << std::endl << url << "\n";
    std::cout << "Found " << extl << " external links.\n";
    std::cout << "Passed " << indexed << " links.\n";
    std::cout << "In queue for crawl " << intl << " internal links.\n";
}

void ViewHandler::ConsoleCrawlerViewer::viewResponseStatus(int code) const
{
    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_100_CONTINUE)
    {
        std::cout << "Response of server: 100 Continue" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_101_SWITCHING_PROTOCOLS)
    {
        std::cout << "Response of server: 101 Switching Protocols" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_200_OK)
    {
        std::cout << "Response of server: 200 OK" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_201_CREATED)
    {
        std::cout << "Response of server: 201 Created" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_202_ACCEPTED)
    {
        std::cout << "Response of server: 202 Accepted" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_203_NON_AUTHORITATIVE_INFORMATION)
    {
        std::cout << "Response of server: 203 Non Authoritative Information" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_204_NO_CONTENT)
    {
        std::cout << "Response of server: 204 No Content" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_205_RESET_CONTENT)
    {
        std::cout << "Response of server: 205 Reset Content" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_206_PARTIAL_CONTENT)
    {
        std::cout << "Response of server: 206 Partial Content" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_300_MULTIPLE_CHOICES)
    {
        std::cout << "Response of server: 300 Multiple Choices" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_301_MOVED_PERMANENTLY)
    {
        std::cout << "Response of server: 301 Moved Permanently" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_302_MOVED_TEMPORARILY)
    {
        std::cout << "Response of server: 302 Moved Temporarily" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_303_SEE_OTHER)
    {
        std::cout << "Response of server: 303 See Other" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_304_NOT_MODIFIED)
    {
        std::cout << "Response of server: 304 Not Modified" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_305_USE_PROXY)
    {
        std::cout << "Response of server: 305 Use Proxy" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_400_BAD_REQUEST)
    {
        std::cout << "Response of server: 400 Bad Request" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_401_UNAUTHORIZED)
    {
        std::cout << "Response of server: 401 Unauthorized" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_402_PAYMENT_REQUIRED)
    {
        std::cout << "Response of server: 402 Payment Required" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_403_FORBIDDEN)
    {
        std::cout << "Response of server: 403 Forbidden" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_404_NOT_FOUND)
    {
        std::cout << "Response of server: 404 Not Found" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_405_METHOD_NOT_ALLOWED)
    {
        std::cout << "Response of server: 405 Method Not Allowed" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_406_NON_ACCEPTABLE)
    {
        std::cout << "Response of server: 406 Non Acceptable" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_407_PROXY_AUTHENTICATION_REQUIRED)
    {
        std::cout << "Response of server: 407 Proxy Authentication Required" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_408_REQUEST_TIMEOUT)
    {
        std::cout << "Response of server: 408 Request Timeout" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_409_CONFLICT)
    {
        std::cout << "Response of server: 409 Conflict" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_410_GONE)
    {
        std::cout << "Response of server: 410 Gone" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_411_LENGTH_REQUIRED)
    {
        std::cout << "Response of server: 411 Length Required" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_412_PRECONDITION_FAILED)
    {
        std::cout << "Response of server: 412 Precondition Failed" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_413_REQUEST_ENTITY_TOO_LARGE)
    {
        std::cout << "Response of server: 413 Request Entity Too Large" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_414_REQUEST_URI_TOO_LONG)
    {
        std::cout << "Response of server: 414 Request URI Too Long" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_415_UNSUPPORTED_MEDIA_TYPE)
    {
        std::cout << "Response of server: 415 Unsupported Media Type" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_500_INTERNAL_SERVER_ERROR)
    {
        std::cout << "Response of server: 500 Internal Server Error" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_501_NOT_IMPLEMENTED)
    {
        std::cout << "Response of server: 501 Not Implemented" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_502_BAD_GATEWAY)
    {
        std::cout << "Response of server: 502 Bad Gateway" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_503_SERVICE_UNAVAILABLE)
    {
        std::cout << "Response of server: 503 Service Unavailable" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_504_GATEWAY_TIMEOUT)
    {
        std::cout << "Response of server: 504 Gateway Timeout" << std::endl;
        return;
    }

    if(code == HttpLib::HttpResponse::CODE_OF_RESPONSE::_505_HTTP_VERSION_NOT_SUPPORTED)
    {
        std::cout << "Response of server: 505 HTTP Version Not Supported" << std::endl;
        return;
    }

    std::cout << "Undefined response of server" << std::endl;
}

void ViewHandler::ConsoleCrawlerViewer::viewWarning(const std::string &warning) const
{
    std::cout << warning;
}
