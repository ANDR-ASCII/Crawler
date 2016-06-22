#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <fstream>
#include "HttpConnection.h"
#include "../Apx/Url.h"

#if PLATFORM == PLATFORM_WINDOWS

// Initialize WinSock2 library
void HttpLib::HttpConnection::init()
{
	if (WSAStartup(0x0202, &libVer_))
	{
		MessageBox(0, "Error", "Error of initialize", MB_OK);
		exit(WSAGetLastError());
	}
}

#endif // PLATFORM

// Close opened socket
void HttpLib::HttpConnection::closeConnection()
{
	if (connectionEstablished_)
	{
		connectionEstablished_ = false;

		#if PLATFORM != PLATFORM_WINDOWS

		close(mainSock_);

		#else

		closesocket(mainSock_);

		#endif
	}
}

// Establishing connection with server that holds the required address
HttpLib::HttpConnection::HttpConnection(const std::string &server)
	: connectionEstablished_(false)
	, mstimeout(DEFAULT_TIMEOUT_MS)
{
    #if PLATFORM == PLATFORM_WINDOWS

    init();

    #endif // PLATFORM

	setConnection(server);
}

HttpLib::HttpConnection::HttpConnection()
	: connectionEstablished_(false)
	, mstimeout(DEFAULT_TIMEOUT_MS)
{
    #if PLATFORM == PLATFORM_WINDOWS

    init();

    #endif // PLATFORM
}

HttpLib::HttpConnection::~HttpConnection()
{
	closeConnection();

    #if PLATFORM == PLATFORM_WINDOWS

    WSACleanup();

    #endif // PLATFORM
}

void HttpLib::HttpConnection::setConnection(const std::string &server)
{
	closeConnection();

	// recreate a socket for new connection
	if ( (mainSock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == CANNOT_CREATE_SOCKET)
	{
	    throw HttpLib::exFailHttp("Fail to create a socket.\n");
	}

	servAddr_.sin_family = AF_INET;
	servAddr_.sin_port = htons(80);

    if(lastServerName != server)
    {
        // caching site name
        lastServerName = server;

        if ( (host_ = gethostbyname(lastServerName.c_str())) )
        {
            servAddr_.sin_addr.s_addr = ((unsigned long **)host_->h_addr_list)[0][0];
        }
        // 1 if successful conversion of address
        else if ((servAddr_.sin_addr.s_addr = inet_addr(lastServerName.c_str())))
        {
            throw HttpLib::exFailHttp("Fail to get IPv4 address of server.\n");
        }
    }

	// 0 - if successful connection
	if (!connect(mainSock_, (sockaddr *)&servAddr_, sizeof(servAddr_)))
	{
		connectionEstablished_ = true;
		return;
	}

    throw HttpLib::exFailHttp("Fail to connect by server.\n");
}

// Sends request by server and returns response
// If object is empty, it means connection was't established
const HttpLib::HttpResponse *HttpLib::HttpConnection::sendRequest(const HttpRequest &request)
{
	if (connectionEstablished_)
	{
		// Clear object of response HTTP server
		response_.clear();

		std::string answerOfServer;
		char buffer[20000];
		int readChars = 0;
		send(mainSock_, request.getResRequest().c_str(), request.getResRequest().size(), 0);

        // synchronous I/O multiplexing
        // used for determine time-out
		FD_ZERO(&testSock);
		FD_SET(mainSock_, &testSock);

		timeval timeout{ 0, mstimeout * 1000 };

		int retval = select(mainSock_ + 1, &testSock, 0, 0, &timeout);

        if(retval == -1)
        {
            throw HttpLib::exFailHttp("multiplexing error.\n");
        }
		if(!retval)
        {
            throw HttpLib::exFailHttp("Time-out.\n");
        }

		// Algorithm for read TCP fragments ====================================================
		while ((readChars = recv(mainSock_, buffer, sizeof(buffer) - 1, 0)) > 0)
		{
			// Insert to end string string buffer, readChars characters
			answerOfServer.insert(answerOfServer.size(), buffer, readChars);
		}

		// =====================================================================================

		response_.setResponse(answerOfServer);
	}

	return &response_;
}

// it clear object response_ anyway
const HttpLib::HttpResponse *HttpLib::HttpConnection::openUrl(const HttpRequest &request)
{
	setConnection(request.getHost());
	sendRequest(request);

	return &response_;
}


void HttpLib::HttpConnection::setTimeOut(long milliseconds)
{
    mstimeout = milliseconds;
}


long HttpLib::HttpConnection::getTimeOut() const
{
    return mstimeout;
}


