#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

// platform detection

#define     PLATFORM_WINDOWS            1
#define     PLATFORM_MAC_OS_X           2
#define     PLATFORM_UNIX               3

#if defined(_WIN32)

#define PLATFORM PLATFORM_WINDOWS

#elif defined(__APPLE__)

#define PLATFORM PLATFORM_MAC_OS_X

#else

#define PLATFORM PLATFORM_UNIX

#endif // platform detection

#if PLATFORM == PLATFORM_WINDOWS

#include <winsock2.h>
#include <ws2tcpip.h>

#define CANNOT_CREATE_SOCKET INVALID_SOCKET

#elif PLATFORM == PLATFORM_MAC_OS_X || PLATFORM == PLATFORM_UNIX

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#define SOCKET int
#define CANNOT_CREATE_SOCKET -1

#endif

#define DEFAULT_TIMEOUT_MS 10000

#include <stdexcept>
#include <string>
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace HttpLib
{
    /**

        Errors of function connect()

        ETIMEDOUT - is returned if on segment SYN no response by server

        ECONNREFUSED - is returned if server on segment SYN answered segment RST
                    This means server node does not contain any process related to listening on specified port.

        ENETUNREACH or EHOSTUNREACH - to the client in response to the SYN segment received ICMP message that unreachable
                                    the recipient from any intermediate routers.
                                    TCP tries send SYN segment three times.
                                    After attempts returns one from above errors.

        ENETUNREACH - is deprecated

    **/

    class exFailHttp
    {
        std::string err_;
    public:
        exFailHttp(const std::string &errWhat) : err_(errWhat) {}
        exFailHttp(const char *errWhat) : err_(errWhat) {}

        virtual const char *what() const
        {
            return err_.c_str();
        }
    };

	class HttpConnection
	{

        #if PLATFORM == PLATFORM_WINDOWS

		WSADATA libVer_;

		// initialize WinSock2
		void init();

		#endif

		SOCKET mainSock_;
		sockaddr_in servAddr_;
		hostent *host_;


		bool connectionEstablished_; // flag of successful connection

        // Caching site name to prevent redundant queries to DNS server
		std::string lastServerName;

		// close opened socket
		void closeConnection();

        // object of response of HTTP server
		HttpResponse response_;

		// structure of file descriptors for test
		// our socket on ready able to read
		fd_set testSock;
		long mstimeout;

	public:

		HttpConnection();
		// connection establish with server which have needed resource
		HttpConnection(const std::string &server);
		~HttpConnection();

        /** ****************** interface ************************* **/

		void setConnection(const std::string &server);
        void setTimeOut(long milliseconds);
        long getTimeOut() const;

		const HttpResponse *openUrl(const HttpRequest &request);
        // send request on server, returns response
		const HttpResponse *sendRequest(const HttpRequest &request);
	};
}

#endif
