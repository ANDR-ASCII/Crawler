#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <iostream>
#include <string>
#include <vector>

namespace HttpLib
{

    /**
    **
    ** by default:
    ** method: GET
    ** relative path: /
    ** version of protocol: HTTP/1.1
    ** connection type: keep-alive
    **
    ***/

	class HttpRequest
	{
	public:

		// methods of HTTP
		enum class METHOD { GET, POST, HEAD };

		// for Connection:
		enum class CONNECTION_TYPE { KEEP_ALIVE, CLOSE };

		enum class VERSION { V_1_0, V_1_1 };

		HttpRequest();

		// in order to assign other objects of the HTTP request
		HttpRequest(const HttpRequest &copyRequest);
		HttpRequest &operator=(const HttpRequest &copyRequest);

		bool operator==(const HttpRequest &cmpRequest);
		bool operator!=(const HttpRequest &cmpRequest);

		// setters
		void	setMethod           (const METHOD method);
		void	setRelativePath     (const std::string &pathToRes);
		void	setHttpVersion      (VERSION httpVersion);
		void	setHost             (const std::string &host);
		void	setUserAgent        (const std::string &userAgent);
		void	setConnectionType   (const CONNECTION_TYPE connectionType);

		bool	addHeader(const std::string &nameOfHeader, const std::string &valueOfHeader);

		// before build new request
		// you must delete old request by calling this method
		void	clear();

		/**
		**
		** method for build request
		** it must call after set methods
		** if method, relative path and version not specified, by default using:
        ** method: GET
        ** relative path: /
        ** version of protocol: HTTP/1.1
        **
        **/

		void					    build();

		// getters
		const	METHOD &            getMethod()			    const;
		const	std::string	&       getRelativePath()	    const;
		VERSION					    getHttpVersion()		const;
		const	std::string	&       getHost()				const;
		const	std::string	&       getUserAgent()			const;
		const	CONNECTION_TYPE	&   getConnectionType()		const;

		const	std::string	&       getResRequest()		    const;

	private:

		std::string			resRequest;			// result request string consisting from HTTP headers
		std::string			reqPath;			// the relative path to the requested resource
		std::string			reqHost;			// www.domain.com
		std::string			reqUserAgent;		// User-Agent header
		VERSION				reqVersion;			// version of HTTP
		METHOD				reqMethod;			// method of request
		CONNECTION_TYPE		reqConnection;      // keep-alive or close

		std::vector<std::string> someHeaders;   // other headers

	};

}

#endif
