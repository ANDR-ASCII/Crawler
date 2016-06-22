#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include "HttpResponse.h"
#include "../Apx/Apx.h"

HttpLib::HttpResponse::HttpResponse()
{
    responseOfServer_ = UNSET;
}

HttpLib::HttpResponse::HttpResponse(const std::string &serverResponse)
{
    responseOfServer_ = UNSET;
	setResponse(serverResponse);
}

void HttpLib::HttpResponse::setResponse(const std::string &serverResponse)
{
	std::string tempStrForHeader;
	auto endOfHeaders = serverResponse.find("\r\n\r\n");
	auto answerLength = serverResponse.size();

	// not found end of header
	if (endOfHeaders == std::string::npos)
	{
		invalidResponse_ = true;
		return;
	}

	for (decltype(endOfHeaders) i = 0; i < endOfHeaders; i++)
	{
		// add header in the vector
		if (serverResponse[i] == '\r' || i == (endOfHeaders - 1))
		{
			// in order not to lose the last character in the last response header
			if (i == (endOfHeaders - 1)) tempStrForHeader += serverResponse[i];

			if (((endOfHeaders - i) > 1 && serverResponse[i + 1] == '\n') || i == (endOfHeaders - 1))
			{
				structOfHeader header;
				auto lengthOfHeader = tempStrForHeader.size(), dblDot = tempStrForHeader.find(':');

				if (dblDot != std::string::npos)
				{
					for (std::string::size_type j = 0; tempStrForHeader[j] != ':'; j++) header.nameHeader += tempStrForHeader[j];
					for (auto j = dblDot + 1; j < lengthOfHeader; j++) header.valueHeader += tempStrForHeader[j];

					Apx::cutSpaceChars(header.nameHeader);
					Apx::cutSpaceChars(header.valueHeader);

					responseHeaders_.push_back(header);
					tempStrForHeader.clear();
				}
				else
				{
					if (tempStrForHeader.find("HTTP") == std::string::npos)
					{
						// unavailable format of response
                        // not found colon character
                        // in the end of header
						invalidResponse_ = true;
						return;
					}
					else
					{
						for (decltype(lengthOfHeader) j = 0; j < lengthOfHeader; j++) header.valueHeader += tempStrForHeader[j];

                        // detects possible errors
                        detectHttpError(header.valueHeader);
						responseHeaders_.push_back(header);
						tempStrForHeader.clear();
					}
				}
			}
			else
			{
			    // unavailable format of response
			    // not found character of newline
			    // in the end of header
				invalidResponse_ = true;
				return;
			}
		}
		else
		{
			// reads header
			tempStrForHeader += serverResponse[i];
		}
	}

    // all headers of http response was read
    // endOfHeaders + 4 - label of start entity body(first character)

    std::string testEncoding;
    getHeader("transfer-encoding", testEncoding);

    if(testEncoding.empty())
	{
	    for (auto i = endOfHeaders + 4; i < answerLength; i++)
        {
            entityBody_ += serverResponse[i];
        }
	}
	else if(testEncoding == "chunked")
    {
        // algorithm of read chunks of entity body
        std::string hexNumber, chunk;

        auto positionOfBeginHexNumber = endOfHeaders + 4;
        auto size = serverResponse.size();

        while(positionOfBeginHexNumber < size)
        {
            auto lblOfEndNumber = serverResponse.find("\r\n", positionOfBeginHexNumber);

            hexNumber.assign(serverResponse, positionOfBeginHexNumber, lblOfEndNumber);
            positionOfBeginHexNumber = lblOfEndNumber + 2;

            // for conversion to valid form for method stoul
            //hexNumber = "0x" + hexNumber;

            // why in code::blocks on windows 7 x64 with compiler GNU GCC 4.8.1
            // i don't use stoul??? Was not declared in this scope, but
            // string library was included
            auto num = std::strtoul(hexNumber.c_str(), 0, 0x10);
            if(!num)
            {
                break;
            }

            chunk.assign(serverResponse, positionOfBeginHexNumber, num);
            entityBody_ += chunk;

            positionOfBeginHexNumber += num + 2;
        }
    }
}

// headers of response
void HttpLib::HttpResponse::getHeaders(std::string &resultHeadersStr) const
{
	resultHeadersStr.clear();
	auto countHeaders = responseHeaders_.size();

	for (decltype(countHeaders) i = 0; i < countHeaders; i++)
	{
		if (responseHeaders_[i].valueHeader.find("HTTP") != std::string::npos)
		{
			resultHeadersStr += responseHeaders_[i].valueHeader + "\r\n";
		}
		else
		{
			resultHeadersStr += responseHeaders_[i].nameHeader + ": " + responseHeaders_[i].valueHeader + "\r\n";
		}

		if ( (countHeaders - i) == 1)
		{
			resultHeadersStr += "\r\n";
		}
	}
}

bool HttpLib::HttpResponse::empty() const
{
    return entityBody_.empty();
}

void HttpLib::HttpResponse::clear()
{
	entityBody_.clear();
	responseHeaders_.clear();
	responseOfServer_ = UNSET;
	invalidResponse_ = false;
}

void HttpLib::HttpResponse::getHeader(const std::string &header, std::string &valueOfHeader) const
{
    valueOfHeader.clear();

	for(const auto &hdr : responseHeaders_)
    {
        if(Apx::strToLower(hdr.nameHeader) == Apx::strToLower(header))
        {
           valueOfHeader = hdr.valueHeader;
           break;
        }
    }
}

bool HttpLib::HttpResponse::isValid() const { return !invalidResponse_; }


/** information codes **/

bool HttpLib::HttpResponse::is_100_Continue() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_100_CONTINUE;
}

bool HttpLib::HttpResponse::is_101_SwitchingProtocols() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_101_SWITCHING_PROTOCOLS;
}

/** successful codes **/

bool HttpLib::HttpResponse::is_200_Ok() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_200_OK;
}

bool HttpLib::HttpResponse::is_201_Created() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_201_CREATED;
}

bool HttpLib::HttpResponse::is_202_Accepted() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_202_ACCEPTED;
}

bool HttpLib::HttpResponse::is_203_NonAuthInformation() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_203_NON_AUTHORITATIVE_INFORMATION;
}

bool HttpLib::HttpResponse::is_204_NoContent() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_204_NO_CONTENT;
}

bool HttpLib::HttpResponse::is_205_ResetContent() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_205_RESET_CONTENT;
}

bool HttpLib::HttpResponse::is_206_PartialContent() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_206_PARTIAL_CONTENT;
}

/** Codes redirection **/

bool HttpLib::HttpResponse::is_300_MultipleChoices() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_300_MULTIPLE_CHOICES;
}

bool HttpLib::HttpResponse::is_301_MovedPermanently() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_301_MOVED_PERMANENTLY;
}

bool HttpLib::HttpResponse::is_302_MovedTemporarily() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_302_MOVED_TEMPORARILY;
}

bool HttpLib::HttpResponse::is_303_SeeOther() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_303_SEE_OTHER;
}

bool HttpLib::HttpResponse::is_304_NotModified() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_304_NOT_MODIFIED;
}

bool HttpLib::HttpResponse::is_305_UseProxy() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_305_USE_PROXY;
}

/** Client errors **/

bool HttpLib::HttpResponse::is_400_BadRequest() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_400_BAD_REQUEST;
    }

bool HttpLib::HttpResponse::is_401_Unauthorized() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_401_UNAUTHORIZED;
}

bool HttpLib::HttpResponse::is_402_PaymentRequired() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_402_PAYMENT_REQUIRED;
}

bool HttpLib::HttpResponse::is_403_Forbidden() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_403_FORBIDDEN;
}

bool HttpLib::HttpResponse::is_404_NotFound() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_404_NOT_FOUND;
}

bool HttpLib::HttpResponse::is_405_MethodNotAllowed() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_405_METHOD_NOT_ALLOWED;
}

bool HttpLib::HttpResponse::is_406_NotAcceptable() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_406_NON_ACCEPTABLE;
}

bool HttpLib::HttpResponse::is_407_ProxyAuthenticationRequired() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_407_PROXY_AUTHENTICATION_REQUIRED;
}

bool HttpLib::HttpResponse::is_408_RequestTimeout() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_408_REQUEST_TIMEOUT;
}

bool HttpLib::HttpResponse::is_409_Conflict() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_409_CONFLICT;
}

bool HttpLib::HttpResponse::is_410_Gone() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_410_GONE;
}

bool HttpLib::HttpResponse::is_411_LengthRequired() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_411_LENGTH_REQUIRED;
}

bool HttpLib::HttpResponse::is_412_PreconditionFailed() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_412_PRECONDITION_FAILED;
}

bool HttpLib::HttpResponse::is_413_RequestEntityTooLarge() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_413_REQUEST_ENTITY_TOO_LARGE;
}

bool HttpLib::HttpResponse::is_414_RequestURITooLong() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_414_REQUEST_URI_TOO_LONG;
}

bool HttpLib::HttpResponse::is_415_UnsupportedMediaType() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_415_UNSUPPORTED_MEDIA_TYPE;
}

/** error codes of server **/

bool HttpLib::HttpResponse::is_500_InternalServerError() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_500_INTERNAL_SERVER_ERROR;
}

bool HttpLib::HttpResponse::is_501_NotImplemented() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_501_NOT_IMPLEMENTED;
}

bool HttpLib::HttpResponse::is_502_BadGateway() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_502_BAD_GATEWAY;
}

bool HttpLib::HttpResponse::is_503_ServiceUnavailable() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_503_SERVICE_UNAVAILABLE;
}

bool HttpLib::HttpResponse::is_504_GatewayTimeout() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_504_GATEWAY_TIMEOUT;
}

bool HttpLib::HttpResponse::is_505_HttpVersionNotSupported() const
{
    return responseOfServer_ == CODE_OF_RESPONSE::_505_HTTP_VERSION_NOT_SUPPORTED;
}

// sets flags of errors if it detected
void HttpLib::HttpResponse::detectHttpError(const std::string &checkStr)
{
    std::string temporary = Apx::strToLower(checkStr);

    // 100 Continue
    if (temporary.find("100") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_100_CONTINUE;
		return;
	}

    // 101 Switching Protocols
    else if (temporary.find("101") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_101_SWITCHING_PROTOCOLS;
		return;
	}

    // 200 OK
    else if (temporary.find("200") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_200_OK;
		return;
	}

    // 201 Created
    else if (temporary.find("201") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_201_CREATED;
		return;
	}

    // 202 Accepted
    else if (temporary.find("202") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_202_ACCEPTED;
		return;
	}

    // 203 Non-Authoritative Information
    else if (temporary.find("203") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_203_NON_AUTHORITATIVE_INFORMATION;
		return;
	}

    // 204 No Content
    else if (temporary.find("204") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_204_NO_CONTENT;
		return;
	}

    // 205 Reset Content
    else if (temporary.find("205") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_205_RESET_CONTENT;
		return;
	}

	// 206 Partial Content
	else if (temporary.find("206") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_206_PARTIAL_CONTENT;
		return;
	}

    // 300 Multiple Choices
    else if (temporary.find("300") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_300_MULTIPLE_CHOICES;
		return;
	}

    // 301 Moved Permanently
	else if (temporary.find("301") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_301_MOVED_PERMANENTLY;
		return;
	}

	// 302 Moved Temporarily
    else if (temporary.find("302") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_302_MOVED_TEMPORARILY;
		return;
    }

    // 303 See Other
    else if (temporary.find("303") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_303_SEE_OTHER;
		return;
    }

    // 304 Not Modified
    else if (temporary.find("304") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_304_NOT_MODIFIED;
		return;
    }

    // 305 Use Proxy
    else if (temporary.find("305") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_305_USE_PROXY;
		return;
    }

    // 400 Bad Request
	else if (temporary.find("400") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_400_BAD_REQUEST;
		return;
    }

    // 401 Unauthorized
	else if (temporary.find("401") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_401_UNAUTHORIZED;
		return;
	}

	// 402 Payment Required
	else if (temporary.find("402") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_402_PAYMENT_REQUIRED;
		return;
	}

    // 403 Forbidden
	else if (temporary.find("403") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_403_FORBIDDEN;
		return;
	}

	// 404 Not Found
	else if (temporary.find("404") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_404_NOT_FOUND;
		return;
	}

    // 405 Method Not Allowed
	else if (temporary.find("405") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_405_METHOD_NOT_ALLOWED;
		return;
	}

	// 406 Not Acceptable
	else if (temporary.find("406") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_406_NON_ACCEPTABLE;
		return;
	}

	// 407 Proxy Authentication Required
	else if (temporary.find("407") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_407_PROXY_AUTHENTICATION_REQUIRED;
		return;
	}

	// 408 Request Timeout
	else if (temporary.find("408") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_408_REQUEST_TIMEOUT;
		return;
	}

	// 409 Conflict
	else if (temporary.find("409") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_409_CONFLICT;
		return;
	}

	// 410 Gone
	else if (temporary.find("410") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_410_GONE;
		return;
	}

	// 411 Length Required
	else if (temporary.find("411") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_411_LENGTH_REQUIRED;
		return;
	}

	// 412 Precondition Failed
	else if (temporary.find("412") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_412_PRECONDITION_FAILED;
		return;
	}

	// 413 Request Entity Too Large
	else if (temporary.find("413") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_413_REQUEST_ENTITY_TOO_LARGE;
		return;
	}

	// 414 Request-URI Too Long
	else if (temporary.find("414") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_414_REQUEST_URI_TOO_LONG;
		return;
	}

	// 415 Unsupported Media Type
	else if (temporary.find("415") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_415_UNSUPPORTED_MEDIA_TYPE;
		return;
	}

	// 500 Internal Server Error
	else if (temporary.find("500") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_500_INTERNAL_SERVER_ERROR;
		return;
	}

	// 501 Not Implemented
	else if (temporary.find("501") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_501_NOT_IMPLEMENTED;
		return;
	}

    // 502 Bad Gateway
	else if (temporary.find("502") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_502_BAD_GATEWAY;
		return;
	}

    // 503 Service Unavailable
	else if (temporary.find("503") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_503_SERVICE_UNAVAILABLE;
		return;
	}

    // 504 Gateway Timeout
	else if (temporary.find("504") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_504_GATEWAY_TIMEOUT;
		return;
	}

    // 505 HTTP Version Not Supported
	else if (temporary.find("505") != std::string::npos)
	{
		responseOfServer_ = CODE_OF_RESPONSE::_505_HTTP_VERSION_NOT_SUPPORTED;
		return;
	}
}
