#include <boost/regex.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Url.h"
#include "Apx.h"

/** ########################################################################################################################### **/

const std::string Apx::Url::extensionsOfExecuteWebFiles[]
{
    "htm",
    "html",
    "php",
    "asp",
    "aspx"
};

boost::regex Apx::Url::statPtnAbsoluteLink(
             "^(https?://|//)?(www\\.)?(?:((?:[\\.\\w-]+?\\.)*))?([\\w-]+)\\."
             "((?:[a-z]{3}\\.[a-z]{2})|(?:[a-z]{2}\\.[a-z]{3})|(?:[a-z]{2}\\.[a-z]{2})|[a-z]{2,6})(:\\d+)?(.*)?$",
             boost::regex_constants::icase
             );

boost::regex Apx::Url::statPtnRelativeLink(
             "^(?:/|//|\\|\\\\)?((?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?(?:/|//|\\|\\\\)?)*"
             "(?:(?:((?:/|//|\\|\\\\)?(?:[[:space:]\\w\\.\\+\\*~!:;-]|%[0-9a-f]{2})+?)(?:\\.(py|asp|php|htm|html|aspx))?)?"
             "(\\?[[:print:]]+?)?)?(?:#.*)?$",
             boost::regex_constants::icase
             );

boost::regex Apx::Url::statExpBasedOnFile(
             "^((?:[/[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})*?)((?:(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+)(?:\\."
             // enumeration extensions of web pages
             "py|asp|php|htm|html|aspx)?)"
             "(?:\\?&?((?:\\w+(?:=?(?:[[:print:]]+)&?)?)*)?)?$",
             boost::regex_constants::icase
             );

boost::regex Apx::Url::statExpBasedOnDirs(
             "^((?:/?(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+)*/?)"
             "(?:/\\?&?((?:\\w+(?:=?(?:[[:print:]]+)&?)?)*)?)?$",
             boost::regex_constants::icase
             );

boost::regex Apx::Url::statSlashes("^/|//|\\|\\\\$");
boost::regex Apx::Url::statAmpersand("&amp;");

/** regular expression for test occurrence on matched file(not web page) **/
/** categories of files                                                 **/
/** #1 archives                                                         **/

boost::regex Apx::Url::statExpArchives(
             "^(?:(?:/|//|\\|\\\\)?(?:(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?)/|//|\\|\\\\)*"
             "(?:/|//|\\|\\\\)?((?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?)\\."
             "("
             // enumeration extensions of archives
             "tar.gz|tar.xz|tar.bz2|tar|tgz|exe|7z|zip|rar|bin|arj|cab|cbr|deb|gzip|jar|one|pak|pkg|ppt|"
             "rpm|sh|sib|sis|sisx|sit|sitx|spl|xar|zipx|gz|xz|bz2|sig|patch"
             ")$",
             boost::regex_constants::icase
             );

/** #2 images **/
boost::regex Apx::Url::statExpImages(
             "^(?:(?:/|//|\\|\\\\)?(?:(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?)/|//|\\|\\\\)*"
             "(?:/|//|\\|\\\\)?(?:[[:space:]\\w\\.\\+:;-]|%[0-9a-f]{2})+?\\."
             "jpg|gif|png|bmp|jpeg"
             "$",
             boost::regex_constants::icase
             );

/** ########################################################################################################################### **/


Apx::Url::Url(bool dContentType)
    : determineContentType_(dContentType)
{
}

Apx::Url::Url(const char *url, bool dContentType)
    : determineContentType_(dContentType)
{
    parse(url);
}

Apx::Url::Url(const std::string &url, bool dContentType)
    : determineContentType_(dContentType)
{
	parse(url);
}

// true if links equivalents
bool Apx::Url::operator==(const Url &cmpUrl) const
{
	if (&cmpUrl == this)
	{
		return true;
	}

    if(   strToLower(getHost())         == strToLower(cmpUrl.getHost())
       && getRelativeDir()              == cmpUrl.getRelativeDir()
       && getFile()                     == cmpUrl.getFile())
    {
        if(variables_.size() != cmpUrl.variables_.size())
        {
            return false;
        }

        // compare variables
        //================================================================
        auto size = variables_.size();
        for(decltype(size) i = 0; i < size; ++i)
        {
            for(decltype(size) j = 0; j < size; ++j)
            {
                // if names of variables or his values not equal => false
                if(variables_[i].first == cmpUrl.variables_[j].first)
                {
                    if(variables_[i].second == cmpUrl.variables_[j].second)
                    {
                        break;
                    }
                    // values not equal
                    else
                    {
                        return false;
                    }
                }

                // variable not found
                if(j == size - 1)
                {
                    return false;
                }
            } // if exit from internal cycle => variable founded and them values equal

        }
        //================================================================

        return true;
    }

	return false;
}

bool Apx::Url::operator!=(const Url &cmpUrl) const
{
	if (*this == cmpUrl)
	{
		return false;
	}

	return true;
}

Apx::Url &Apx::Url::operator=(const char *url)
{
	parse(url);
	return *this;
}

Apx::Url &Apx::Url::operator=(const std::string &url)
{
	parse(url);
	return *this;
}

void Apx::Url::parse(const std::string &testLnk)
{
    clear();

    std::string url = testLnk;
    Apx::cutSpaceChars(url);

    if(url.find("javascript:") != std::string::npos || url.find("mailto:") != std::string::npos)
    {
        return;
    }

    try
    {
        if (url.empty())
        {
            return;
        }

        boost::smatch partsOfLink;

        // if it anchor
        if (url[0] == '#')
        {
            nameAnchor_ = url;
            anchor_ = true;
            return;
        }


        // this is absolute address, if matched pattern and last part after '.' is not a web executing extension
        // because .html extension also match with: [a-z]{2,6}|[a-z]{3}\\.[a-z]{2}|[a-z]{2}\\.[a-z]{3}|[a-z]{2}\\.[a-z]{2}
        absoluteAddr_ = boost::regex_match(url, partsOfLink, statPtnAbsoluteLink) && !isExtMatchWebPage(partsOfLink[5]);

        if (absoluteAddr_)
        {
            protocol_ = (partsOfLink[1].length() && partsOfLink[1] != "//") ? partsOfLink[1] : std::string("http://");
            path_ = partsOfLink[7].length() ? partsOfLink[7] : std::string("/");

            // replace XML present ampersand &amp; to equivalent &
            ampToSymb();

            // conversion to the format: [www.][subdomains]site.com
            host_ = partsOfLink[2] + partsOfLink[3] + partsOfLink[4] + "." + partsOfLink[5];
            topLevelDomain_ = partsOfLink[5];

            // separate directory
            if (relativeDir_ != "/")
            {
                // remove anchor
                removeAnchor();
                parseRelativePath();
            }

            return;
        }

        // ==================================================================================

        // if it relative path
        relativeAddr_ = boost::regex_search(url, statPtnRelativeLink);

        if (relativeAddr_)
        {
            // separate directory
            path_ = url;

            // replace XML present ampersand &amp; to equivalent &
            ampToSymb();

            // remove anchor
            removeAnchor();
            parseRelativePath();
        }
    }
    catch(std::runtime_error &rte)
    {
        // catch exception generated by boost library
        // add reference which throws an exception
        throw Apx::exErrorUrl(rte.what(), url);
    }
}

// remove anchor from relative path if it exists: path_
// otherwise nothing not do
void Apx::Url::removeAnchor()
{
    // remove anchor
    std::string::size_type anchorIndex = path_.rfind('#');

    if(anchorIndex != std::string::npos)
    {
        nameAnchor_.assign(path_, anchorIndex, std::string::npos);
        path_.assign(path_, 0, anchorIndex);
    }
}

// parsing relative path by parts
void Apx::Url::parseRelativePath()
{
        boost::smatch firstCmp, sm;

        /**
        ** NOTE:
        **      /some  - some is a file without extension.
        **      /some/ - some is a folder.
        **/

        // If file found
        if((boost::regex_search(path_, firstCmp, statExpBasedOnFile)))
        {
            if(determineContentType_)
            {
                // #1
                if( boost::regex_search(path_, sm, statExpArchives))
                {
                    // write name of file with extension
                    file_ = sm[1] + '.' + sm[2];
                    extensionOfFile_ = sm[2];

                    relativeDir_.assign(path_, 0, path_.find(file_));
                    type_ = TYPE_OF_FILE::ARCHIVE;
                    return;
                }
                // #2
                else if(boost::regex_search(path_, sm, statExpImages))
                {
                    // write name of file with extension
                    file_ = sm[1] + '.' + sm[2];
                    extensionOfFile_ = sm[2];

                    relativeDir_.assign(path_, 0, path_.find(file_));
                    type_ = TYPE_OF_FILE::IMAGE;
                    return;
                }
            }

            // obtains the directories and get variables
            file_ = boost::regex_replace(path_, statExpBasedOnFile, "$2");

            std::string::size_type lbl = std::string::npos;
            if( (lbl = file_.find(".")) != std::string::npos)
            {
                extensionOfFile_.assign(file_, lbl + 1, std::string::npos);
            }

            if(type_ == TYPE_OF_FILE::UNDEFINED)
            {
                // if after all tests type of extensions not set
                // then set type as EXECUTE_WEB_FILE

                // such behavior by default
                type_ = TYPE_OF_FILE::EXECUTE_WEB_FILE;
            }

            // if file found but can not be found folders or/and get variables
            relativeDir_ = boost::regex_replace(path_, statExpBasedOnFile, "$1");
            variablesString_ = boost::regex_replace(path_, statExpBasedOnFile, "$3");

            if(!variablesString_.empty())
            {
                parseGetVars(variablesString_);
                variablesString_ = "?" + variablesString_;
            }

            if(relativeDir_ == path_)
            {
                relativeDir_.clear();
            }

        }
        else
        {
            // if file not found

            // if found folder or get variables
            if(boost::regex_search(path_, sm, statExpBasedOnDirs))
            {
                relativeDir_ = sm[1];
                variablesString_ = sm[2];

                if(!variablesString_.empty())
                {
                    parseGetVars(variablesString_);
                    variablesString_ = "?" + variablesString_;
                    relativeDir_.append("/");
                }
            }
            else
            {
                throw std::runtime_error(
                                         "Error when parsing the relative path.\n"
                                         "File, folders and GET variables not found."
                                         );
            }
        }
}

//
void Apx::Url::parseGetVars(const std::string &vars)
{
    boost::regex exp("(.*?)=(.*)");

    std::string::size_type label = vars.find('&'), begin = 0, size = vars.size();

    while(label != std::string::npos)
    {
        std::string var_val = vars.substr(begin, label - begin);
        begin = ++label;

        // if var have value
        if(boost::regex_search(var_val, exp))
        {
            std::string tmpVarName, tmpVarValue;
            tmpVarName = boost::regex_replace(var_val, exp, "$1");
            tmpVarValue = boost::regex_replace(var_val, exp, "$2");

            variables_.push_back(std::make_pair(Apx::cutSpaceChars(tmpVarName), Apx::cutSpaceChars(tmpVarValue)));
        }

        label = vars.find('&', begin);
    }

    std::string var_val = vars.substr(begin, std::string::npos);
    // if var have value
    if(boost::regex_search(var_val, exp))
    {
        std::string tmpVarName, tmpVarValue;
        tmpVarName = boost::regex_replace(var_val, exp, "$1");
        tmpVarValue = boost::regex_replace(var_val, exp, "$2");

        variables_.push_back(std::make_pair(Apx::cutSpaceChars(tmpVarName), Apx::cutSpaceChars(tmpVarValue)));
    }
}

void Apx::Url::clear()
{
    protocol_.clear();
    host_.clear();
    path_.clear();
    relativeDir_.clear();
    file_.clear();
    nameAnchor_.clear();
    variables_.clear();

    // reset flags
    anchor_ = absoluteAddr_ = relativeAddr_ = false;
}

// it compares *this host with cmpLnk host
// if second parameter is true then subdomain.domain.com and www.subdomain.domain.com will be equivalent
bool Apx::Url::compareHost(const Url &cmpLnk) const
{
    if (&cmpLnk == this)
	{
		return true;
	}

	if (strToLower(host_) == strToLower(cmpLnk.host_))
	{
		return true;
	}

	return false;
}

// replaces &amp; to & in relative path
void Apx::Url::ampToSymb()
{
    if(!path_.empty())
    {
        path_ = boost::regex_replace(path_, statAmpersand, "&");
    }
}

// this method need for compare extension on match with extensions
// of web pages such as - htm, html, php, asp, aspx(execute files)
bool Apx::Url::isExtMatchWebPage(const std::string &extension)
{
    for(const auto &ext : extensionsOfExecuteWebFiles)
    {
        if(ext == extension)
        {
            return true;
        }
    }

    return false;
}

// true if specify variable exists otherwise false
int Apx::Url::isSetVariable(const std::string &nameOfVar)
{
    auto size = variables_.size();
    for(decltype(size) i = 0; i < size; ++i)
    {
        if(variables_[i].first == nameOfVar)
        {
            return i;
        }
    }

    return -1;
}


// returns value of specify variable if success
// otherwise returns empty string
std::string Apx::Url::getValOfVar(const std::string &nameOfVar)
{
    for(const auto &somePair : variables_)
    {
        if(somePair.first == nameOfVar)
        {
            return somePair.second;
        }
    }

    return "";
}

// returns value of variable by number 'num' if success
// otherwise returns empty string
std::string Apx::Url::getValOfVar(std::size_t num)
{
    auto size = variables_.size();

    if(num > size)
    {
        return "";
    }

    return variables_[num].second;
}

void Apx::Url::determineContentType(bool dContentType)
{
    determineContentType_ = dContentType;
}
