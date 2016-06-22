#include "HttpRequest.h"
#include "../Apx/Apx.h"
#include "../Apx/Url.h"

HttpLib::HttpRequest::HttpRequest()
    : reqVersion(VERSION::V_1_1)
    , reqMethod(METHOD::GET)
    , reqConnection(CONNECTION_TYPE::KEEP_ALIVE)
    {}

HttpLib::HttpRequest::HttpRequest(const HttpRequest &copyRequest)
{
	resRequest = copyRequest.resRequest;
	reqConnection = copyRequest.reqConnection;
	reqHost = copyRequest.reqHost;
	reqMethod = copyRequest.reqMethod;
	reqPath = copyRequest.reqPath;
	reqUserAgent = copyRequest.reqUserAgent;
	reqVersion = copyRequest.reqVersion;
}

HttpLib::HttpRequest &HttpLib::HttpRequest::operator=(const HttpRequest &copyRequest)
{
	if (*this == copyRequest)
		return *this;

	resRequest = copyRequest.resRequest;
	reqConnection = copyRequest.reqConnection;
	reqHost = copyRequest.reqHost;
	reqMethod = copyRequest.reqMethod;
	reqPath = copyRequest.reqPath;
	reqUserAgent = copyRequest.reqUserAgent;
	reqVersion = copyRequest.reqVersion;

	return *this;
}

bool HttpLib::HttpRequest::operator==(const HttpRequest &cmpRequest)
{
	if (resRequest == cmpRequest.resRequest &&
		reqConnection == cmpRequest.reqConnection &&
		reqHost == cmpRequest.reqHost &&
		reqMethod == cmpRequest.reqMethod &&
		reqPath == cmpRequest.reqPath &&
		reqUserAgent == cmpRequest.reqUserAgent &&
		reqVersion == cmpRequest.reqVersion)

			return true;

	return false;
}

bool HttpLib::HttpRequest::operator!=(const HttpRequest &cmpRequest)
{
	if (*this == cmpRequest)
		return false;

	return true;
}

// setters
void HttpLib::HttpRequest::setMethod(const METHOD method)
{
	reqMethod = method;
}

void HttpLib::HttpRequest::setRelativePath(const std::string &pathToRes)
{
	reqPath = pathToRes;
}

void HttpLib::HttpRequest::setHttpVersion(VERSION httpVersion)
{
	reqVersion = httpVersion;
}

void HttpLib::HttpRequest::setHost(const std::string &host)
{
	reqHost = host;
}

void HttpLib::HttpRequest::setUserAgent(const std::string &userAgent)
{
	reqUserAgent = userAgent;
}

void HttpLib::HttpRequest::setConnectionType(const CONNECTION_TYPE connectionType)
{
	reqConnection = connectionType;
}

bool HttpLib::HttpRequest::addHeader(const std::string &nameOfHeader, const std::string &valueOfHeader)
{
	if (!nameOfHeader.empty() && !valueOfHeader.empty())
	{
		someHeaders.push_back(nameOfHeader + ": " + valueOfHeader + "\r\n");
		return true;
	}

	return false;
}

// method for build request
// it must call after set methods
void HttpLib::HttpRequest::build()
{
	int someHdrsCount = 0;

	// configurations by default
	if (reqPath.size())
	{
		if (reqPath[0] != '/')
		{
			std::string tmp = reqPath;
			reqPath = "/" + tmp;
		}
	}
	else
	{
		reqPath = "/";
	}

    // build request
    // main header
    resRequest = reqMethod == METHOD::GET ? "GET " : reqMethod == METHOD::POST ? "POST " : "HEAD ";
    resRequest += reqPath + " " + (reqVersion == VERSION::V_1_0 ? "HTTP/1.0" : "HTTP/1.1") + "\r\n";

    resRequest += "Host: " + reqHost + "\r\n";

    if (!reqUserAgent.empty())
    {
        resRequest += "User-Agent: " + reqUserAgent + "\r\n";
    }

    if (reqConnection == CONNECTION_TYPE::CLOSE)
    {
        resRequest += "Connection: close\r\n";
    }
    else
    {
        resRequest += "Connection: keep-alive\r\n";
    }

    //
    if ((someHdrsCount = someHeaders.size()))
    {
        for (int i = 0; i < someHdrsCount; i++)
        {
            resRequest += someHeaders[i];
        }
    }

    resRequest += "\r\n";
}

// getters
const HttpLib::HttpRequest::METHOD &HttpLib::HttpRequest::getMethod() const
{
	return reqMethod;
}

const std::string &HttpLib::HttpRequest::getRelativePath() const
{
	return reqPath;
}

HttpLib::HttpRequest::VERSION HttpLib::HttpRequest::getHttpVersion() const
{
	return reqVersion;
}

const std::string &HttpLib::HttpRequest::getHost() const
{
	return reqHost;
}

const std::string &HttpLib::HttpRequest::getUserAgent() const
{
	return reqUserAgent;
}

const HttpLib::HttpRequest::CONNECTION_TYPE & HttpLib::HttpRequest::getConnectionType() const
{
	return reqConnection;
}

const std::string &HttpLib::HttpRequest::getResRequest() const
{
	return resRequest;
}

void HttpLib::HttpRequest::clear()
{
	reqConnection = CONNECTION_TYPE::KEEP_ALIVE;
	reqVersion = VERSION::V_1_1;
	reqMethod = METHOD::GET;

	resRequest.clear();
	reqHost.clear();
	reqPath.clear();
	reqUserAgent.clear();
	someHeaders.clear();
}
