#ifndef HTTP_ANSWER_H
#define HTTP_ANSWER_H

#include <iostream>
#include <string>
#include <vector>

namespace HttpLib
{
	struct structOfHeader
	{
		std::string nameHeader;
		std::string valueHeader;
	};

    /**
    **
    ** This object create and edit may only HttpConnection objects.
    ** Also anyone can create this object using copy constructor.
    ** But cannot change, if only using the operator =.
    ** This features allows to keep an object in an other object if you need to store data.
    **
    **/

	class HttpResponse
	{
    public:

        enum CODE_OF_RESPONSE
        {
              _100_CONTINUE
            , _101_SWITCHING_PROTOCOLS
            //=================================
            , _200_OK
            , _201_CREATED
            , _202_ACCEPTED
            , _203_NON_AUTHORITATIVE_INFORMATION
            , _204_NO_CONTENT
            , _205_RESET_CONTENT
            , _206_PARTIAL_CONTENT
            //=================================
            , _300_MULTIPLE_CHOICES
            , _301_MOVED_PERMANENTLY
            , _302_MOVED_TEMPORARILY
            , _303_SEE_OTHER
            , _304_NOT_MODIFIED
            , _305_USE_PROXY
            //=================================
            , _400_BAD_REQUEST
            , _401_UNAUTHORIZED
            , _402_PAYMENT_REQUIRED
            , _403_FORBIDDEN
            , _404_NOT_FOUND
            , _405_METHOD_NOT_ALLOWED
            , _406_NON_ACCEPTABLE
            , _407_PROXY_AUTHENTICATION_REQUIRED
            , _408_REQUEST_TIMEOUT
            , _409_CONFLICT
            , _410_GONE
            , _411_LENGTH_REQUIRED
            , _412_PRECONDITION_FAILED
            , _413_REQUEST_ENTITY_TOO_LARGE
            , _414_REQUEST_URI_TOO_LONG
            , _415_UNSUPPORTED_MEDIA_TYPE
            //=================================
            , _500_INTERNAL_SERVER_ERROR
            , _501_NOT_IMPLEMENTED
            , _502_BAD_GATEWAY
            , _503_SERVICE_UNAVAILABLE
            , _504_GATEWAY_TIMEOUT
            , _505_HTTP_VERSION_NOT_SUPPORTED
        };

    private:

        const int UNSET = -1; // for flag responseOfServer_

		std::vector<structOfHeader> responseHeaders_;	// headers of response of server
		std::string entityBody_;					    // entity body obtained from response of server

		bool invalidResponse_;						    // flag of validation of response
        int responseOfServer_;

        // the class whose objects can create objects of this type
        friend class HttpConnection;

        HttpResponse();
		HttpResponse(const std::string &serverResponse); // in moment of create object must to initialize of response of server
		void setResponse(const std::string &serverResponse);

		void readChunks(const std::string &entityBody);

        // sets flags of errors if it detected
		void detectHttpError(const std::string &checkStr);

	public:

		bool isValid()                            const;

		/** is_ERRNUM_Transcription() **/

        /** information codes **/
        bool is_100_Continue()                    const;
        bool is_101_SwitchingProtocols()          const;

        /** Successful codes **/
        bool is_200_Ok()                          const;
        bool is_201_Created()                     const;
        bool is_202_Accepted()                    const;
        bool is_203_NonAuthInformation()          const;
        bool is_204_NoContent()                   const;
        bool is_205_ResetContent()                const;
        bool is_206_PartialContent()              const;

        /** Codes redirection **/
        bool is_300_MultipleChoices()             const;
        bool is_301_MovedPermanently()            const;
        bool is_302_MovedTemporarily()            const;
        bool is_303_SeeOther()                    const;
        bool is_304_NotModified()                 const;
        bool is_305_UseProxy()                    const;

        /** Client errors **/
        bool is_400_BadRequest()                  const;
        bool is_401_Unauthorized()                const;
        bool is_402_PaymentRequired()             const;
        bool is_403_Forbidden()                   const;
        bool is_404_NotFound()                    const;
        bool is_405_MethodNotAllowed()            const;
        bool is_406_NotAcceptable()               const;
        bool is_407_ProxyAuthenticationRequired() const;
        bool is_408_RequestTimeout()              const;
        bool is_409_Conflict()                    const;
        bool is_410_Gone()                        const;
        bool is_411_LengthRequired()              const;
        bool is_412_PreconditionFailed()          const;
        bool is_413_RequestEntityTooLarge()       const;
        bool is_414_RequestURITooLong()           const;
        bool is_415_UnsupportedMediaType()        const;

        /** Error codes of server **/
        bool is_500_InternalServerError()         const;
        bool is_501_NotImplemented()              const;
        bool is_502_BadGateway()                  const;
        bool is_503_ServiceUnavailable()          const;
        bool is_504_GatewayTimeout()              const;
        bool is_505_HttpVersionNotSupported()     const;


		bool empty() const;
		void clear();

		/** ****************** getters ****************** **/

        int getCodeOfResponse()             const   { return responseOfServer_; }
		const std::string &getEntityBody()  const   { return entityBody_;       }

		// get all headers as string object like returns HEAD method
		void getHeaders(std::string &resultHeadersStr) const;

		// get specified header
		void getHeader(const std::string &header, std::string &valueOfHeader) const;

	};

}

#endif
