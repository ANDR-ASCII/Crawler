#ifndef WEBLINK_H_INCLUDED
#define WEBLINK_H_INCLUDED

#include <boost/regex.hpp>
#include <stdexcept>
#include <utility>
#include <vector>

namespace Apx
{

    /**********************************************************************************************
	 *
	 * #1. Object of class Link when parse the supplied address into parts
     * saves the relative path:
     *
     * 1) if passed address is an absolute link with the slash at the beginning of the address.
     *
     * 2) if the passed address is a relative link: in the form in which the parser pulled it out of the tag "a".
     *
     * getRelativeDir()  returns levels of directories to the website root.
     * getRelativePath() returns full path to the file from the website root.
     *
     * Example:
     *
     * If function accepts absolute link: www.site.ru/dir1/dir2/index.php
     * getRelativeDir()  returns - /dir1/dir2/
     * getRelativePath() returns - /dir1/dir2/index.php
     *
     *
     * If function accepts relative link: /dir1/dir2/index.php
     * getRelativeDir()  returns - /dir1/dir2/
     * getRelativePath() returns - /dir1/dir2/index.php
     *
     *
     * If function accepts relative link: dir1/dir2/index.php
     * getRelativeDir()  returns - dir1/dir2/
     * getRelativePath() returns - dir1/dir2/index.php
     *
     *
     * If function accepts relative link: dir1/dir2
     * getRelativeDir()  returns - dir1/dir2
     * getRelativePath() returns - dir1/dir2
     *
     * ===============================================================================================
     *
     * Second case:
     * such a behavior is needed for appropriate handling
     * result address converted by Crawler's functions.
     *
     * For which an important role is played by the first slash in the address
     * obtained by the parser from html page.
     * Thus, if found link with relative path of the following kind:
     *
     *            /folder1[/[folder2[/[index.php[?id=1&cat=1]]]]]
     *
     * It does not matter which address you received this link, it is saved
     * in turn relative links like this for what it is.
     *
     * If in relative link first character not a slash, then
     * key role in conversion right address played
     * relative path from was get this address.
     *
     */

    class exErrorUrl : public std::runtime_error
    {
        std::string badUrl_;
    public:
        exErrorUrl(const std::string &details, const std::string &badUrl)
                : std::runtime_error(details)
                , badUrl_(badUrl)
                {}

        exErrorUrl(const char *details, const char *badUrl)
                : std::runtime_error(details)
                , badUrl_(badUrl)
                {}

        virtual ~exErrorUrl() {}

        virtual const char *what() const noexcept
        {
            return ("Crash regex algorithm when parsing URL: " + badUrl_ + "\n"
                    "===============================================================================\n"
                    "Details: " + std::runtime_error::what() + "\n"
                    "###############################################################################\n\n\n").c_str();
        }
    };


	class Url
	{
    public:

        enum class TYPE_OF_FILE
        {
              UNDEFINED       // used by default and for files without extensions
            , IMAGE
            , ARCHIVE
            , DOCUMENT
            , TEXT
            , AUDIO
            , VIDEO
            , EXECUTE_WEB_FILE  // all files after which can be character '?' and variables
        };

        // ctors
        Url(bool parseCatFile = true);
		Url(const char *lnk, bool parseCatFile = true);
		Url(const std::string &lnk, bool parseCatFile = true);

		// comparison addresses

		// true if links equivalents
		bool operator==(const Url &cmpLnk) const;
		bool operator!=(const Url &cmpLnk) const;

        operator bool() const { return (anchor_ || absoluteAddr_ || relativeAddr_); }

        Url &operator=(const char *lnk);
		Url &operator=(const std::string &lnk);

		void clear();

		// check the link
		// before call this method must call clear()
		void parse(const std::string &testLnk);

        // it compares *this host with cmpLnk host
        // if second parameter is true then domain.com and www.domain.com will be equivalent
		bool compareHost(const Url &cmpLnk)    const;

        // returns number of element in vector of variables
        // if specify var was found, otherwise -1
		int isSetVariable(const std::string &nameOfVar);

		// set flag for determine content type based on file
		void determineContentType(bool dContentType);

        // returns value of specify variable if success
        // otherwise returns empty string
		std::string getValOfVar(const std::string &nameOfVar);

		// returns value of variable by number 'num' if success
        // otherwise returns empty string
		std::string getValOfVar(std::size_t num);

		/** **** INLINE METHODS **** **/

        // check the status link
        bool isAnchor()                         const  { return anchor_;              }
		bool isAbsoluteAddr()                   const  { return absoluteAddr_;        }
		bool isRelativeAddr()                   const  { return relativeAddr_;        }

		// getters

		// returns string like this: [www.] [subdomain.] domain.com
		const TYPE_OF_FILE &getTypeOfFile()       const  { return type_;                }
		const std::string  &getExtenstionOfFile() const  { return extensionOfFile_;     }
		const std::string  &getProtocol()         const  { return protocol_;            }
		const std::string  &getHost()             const  { return host_;                }
		const std::string  &getRelativePath()     const  { return path_;                }
		const std::string  &getRelativeDir()      const  { return relativeDir_;         }
		const std::string  &getFile()             const  { return file_;                }
		const std::string  &getAnchor()           const  { return nameAnchor_;          }
		const std::string  &getTopLevelDomain()   const  { return topLevelDomain_;      }

		// returns string appropriate get variables: ?variable_1=value_1&variable_2=value_2
		// empty if variables not exists
		const std::string &getVariablesString()   const  { return variablesString_;     }

//===========================================================================================
    private:

		// check status link
		bool anchor_;       // sets if the link is anchor: #name_anchor
                            // but not set if anchor occurred in the end of the link

		bool absoluteAddr_; // sets if the link is absolute address:
                            // [www.] [subdomain.] domain.com [ [/folder1/folder2/] page.php [? [var1=val1&var2=val2] ] ]

		bool relativeAddr_; // sets if the link is relative path: [/folder1/folder2/] page.php [? [var1=val1&var2=val2] ]

		bool determineContentType_; // set flag for determine content type based on file

		// parse link
		std::string protocol_;
		std::string host_;
		std::string path_;
		std::string relativeDir_;
		std::string file_;
		std::string nameAnchor_;
        std::string extensionOfFile_;
        std::string variablesString_;
        std::string topLevelDomain_;
        TYPE_OF_FILE type_ = TYPE_OF_FILE::UNDEFINED;

        // consists name of var and value of var
		std::vector<std::pair<std::string, std::string> > variables_;

		// replaces &amp; to & in relative path
		void ampToSymb();


		/** handling of possible errors **/

        // parsing relative path by parts
        // must be called after handling of possible errors
        void parseRelativePath();
        void parseGetVars(const std::string &vars);

        // remove anchor from relative path if it exists: path_
        void removeAnchor();

        // static regex objects, for compile once
        static boost::regex statPtnAbsoluteLink;
        static boost::regex statPtnRelativeLink;
        static boost::regex statExpBasedOnFile;
        static boost::regex statExpBasedOnDirs;
        static boost::regex statSlashes;
        static boost::regex statAmpersand;

        // regular expression for test occurrence on matched file(not web page)
        static boost::regex statExpArchives;    // for detect archives
        static boost::regex statExpImages;      // for detect images

        static const std::string extensionsOfExecuteWebFiles[];

        // this method need for compare extension on match with extensions
        // of web pages such as - htm, html, php, asp, aspx(execute files)
        // also it need for compare relative link such a - index.html
        // because such a relative link matched for "statPtnAbsoluteLink"
        bool isExtMatchWebPage(const std::string &extension);
	};

}

#endif // WEBLINK_H_INCLUDED
