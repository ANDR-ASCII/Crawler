#include <boost/regex.hpp>
#include <fstream>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include "GeneralHelperFunctions.h"
#include "Crawler.h"
#include "Apx/Apx.h"

// correct time
#define STD_PAUSE_BETWEEN_REQUESTS 3000

const std::string CrawlerImpl::Crawler::storage_of_phrases = "phrases.txt";
const std::string CrawlerImpl::Crawler::report_url = "url_crash_report.log";
const std::string CrawlerImpl::Crawler::storage_urls = "table_of_addresses.ta";

CrawlerImpl::Crawler::Crawler()
	: timePause_(STD_PAUSE_BETWEEN_REQUESTS)
	, sign_("PasBot v1.0 Alpha ")
    , maxDepth_(0)
    , readyForStart_(false)
    , infiniteCrawl_(false)
{
    signature_ = sign_;
	request_.setUserAgent(signature_);
}

CrawlerImpl::Crawler::Crawler(const std::string &startAddr)
    : timePause_(STD_PAUSE_BETWEEN_REQUESTS)
    , sign_("PasBot v1.0 Alpha ")
    , maxDepth_(0)
    , infiniteCrawl_(false)
{
	signature_ = sign_;
	setStartAddress(startAddr);
	request_.setUserAgent(signature_);
}

CrawlerImpl::Crawler::Crawler(const std::string &startAddr, std::size_t timePause)
    : timePause_(timePause)
    , sign_("PasBot v1.0 Alpha ")
    , maxDepth_(0)
    , infiniteCrawl_(false)
{
    signature_ = sign_;
	setStartAddress(startAddr);
	request_.setUserAgent(signature_);
}

void CrawlerImpl::Crawler::crawlStart()
{
	Apx::TagParser parser;

    // send on output device
    viewerDevice_->viewInfo(host_.getHost(),
                            externalLinks_.size(),
                            internalLinks_.size(),
                            indexedInternal_.size());

    // set configurations of request
	request_.setHost(host_.getHost());
	request_.setHttpVersion(HttpLib::HttpRequest::VERSION::V_1_1);
	request_.setConnectionType(HttpLib::HttpRequest::CONNECTION_TYPE::CLOSE);

	// stores the indexed link
    indexedInternal_.push_back("/");

	try
    {
        // for correct output
        // if not set, print 200 OK, otherwise not
        bool mpOccur = false;
        SLEEP(timePause_);

        request_.build();
        // get address of object response_ of controller
        response_ = controller_.openUrl(request_);

        if (response_->isValid())
        {
            // HTTP error - 301 Moved Permanently
            while(response_->is_301_MovedPermanently() || response_->is_302_MovedTemporarily())
            {
                viewerDevice_->viewResponseStatus(response_->getCodeOfResponse());
                addLinkToQueue(host_, INDEXED_EX); // save current url

                // set flag
                mpOccur = !mpOccur;

                std::string tmpRefer, firstHost = host_.getHost();
                response_->getHeader("location", tmpRefer);
                host_.parse(tmpRefer);

                // until HttpLib don't support https
                if(host_.getProtocol() == "https://")
                {

                    viewerDevice_->viewWarning("****************************************************\n"
                                               "* To address this need support Https.              *\n"
                                               "* But HttpLib still does not support this protocol.*\n"
                                               "****************************************************\n");


                    // jump into crawlResource with empty parameters
                    // for the next crawl
                    if(getMaxDepth() > 1)
                    {
                        crawlResource();
                    }
                }

                if(host_.isAbsoluteAddr())
                {
                    request_.setHost(host_.getHost());
                    request_.setRelativePath(host_.getRelativePath());
                }
                else if(host_.isRelativeAddr())
                {
                    // in order not to lose the host
                    host_ = firstHost + convertRelativeAddr(host_.getRelativePath(), "/");
                    request_.setRelativePath(host_.getRelativePath());
                }

                request_.build();

                // get address of object response_ of controller
                response_ = controller_.openUrl(request_);

                // send on output device
                viewerDevice_->viewInfo(host_.getHost(),
                                       externalLinks_.size(),
                                       internalLinks_.size(),
                                       indexedInternal_.size());
            }

            viewerDevice_->viewResponseStatus(response_->getCodeOfResponse());

            // saves links
            parser.parseTags(response_->getEntityBody(), "a");
            savePage(indexedInternal_.back(), Apx::cutText(response_->getEntityBody()));
            addToQueue(parser, indexedInternal_.back());

            if(getMaxDepth() == 1)
            {
                return;
            }

            // calls indexer of website
            crawlResource();
        }
        else
        {
            viewerDevice_->viewWarning("Response invalid\n");
        }
    }
    catch(HttpLib::exFailHttp &fh)
    {
        std::cout << "Web site unavailable.\n";
        std::ofstream fails("failConnections.log", std::ios_base::app);
        fails << "Address: " << request_.getHost() + request_.getRelativePath()
                << std::endl << fh.what() << std::endl;

        if(getMaxDepth() > 1)
        {
            crawlResource();
        }
        else
        {
            return;
        }
    }
    catch(Apx::exErrorUrl &err_url)
    {
        // If failed parse link and object of Link throws exception
        // We create report consisting from address
        // And continues data handling

        std::ofstream crash(report_url, std::ios_base::app);
        crash << err_url.what() << std::endl;
    }
}

void CrawlerImpl::Crawler::crawlResource()
{
	Apx::TagParser parser;

    std::size_t i = 0;

	while (internalLinks_.size())
	{
	    // send on output device
        viewerDevice_->viewInfo(host_.getHost() + internalLinks_.front(),
                                externalLinks_.size(),
                                internalLinks_.size(),
                                indexedInternal_.size());

		// build request
		request_.setHttpVersion(HttpLib::HttpRequest::VERSION::V_1_1);
		request_.setRelativePath(internalLinks_.front());
		request_.setConnectionType(HttpLib::HttpRequest::CONNECTION_TYPE::CLOSE);

		// stores the indexed link
		indexedInternal_.push_back(internalLinks_.front());
		internalLinks_.pop_front();

        try
        {
            // wait enter and in the time this is pause between http requests
            SLEEP(timePause_);

            request_.build();
            response_ = controller_.openUrl(request_);

            if (response_->isValid())
            {
                if (response_->is_301_MovedPermanently() || response_->is_302_MovedTemporarily())
                {
                    // read header Location
                    std::string tmpRefer;
                    response_->getHeader("location", tmpRefer);

                    // test link
                    Apx::Url url(tmpRefer);

                    // until HttpLib don't support https
                    if(host_.getProtocol() == "https://")
                    {
                        viewerDevice_->viewWarning("****************************************************\n"
                                                   "* To address this need support Https.              *\n"
                                                   "* But HttpLib still does not support this protocol.*\n"
                                                   "****************************************************\n");
                        continue;
                    }

                    // if it same host
                    if(url.compareHost(host_))
                    {
                        addLinkToQueue(url, CrawlerImpl::Crawler::INTERNAL);
                    }
                    else
                    {
                        addLinkToQueue(url, CrawlerImpl::Crawler::EXTERNAL);
                    }

                    viewerDevice_->viewResponseStatus(response_->getCodeOfResponse());
                }
                else
                {
                    // saves unique links
                    viewerDevice_->viewResponseStatus(response_->getCodeOfResponse());

                    parser.parseTags(response_->getEntityBody(), "a");
                    savePage(indexedInternal_.back(), Apx::cutText(response_->getEntityBody()));
                    addToQueue(parser, Apx::Url(indexedInternal_.back()));
                }
            }
            else
            {
                viewerDevice_->viewWarning("Response of server invalid\n");
            }

        }
        catch(HttpLib::exFailHttp &fh)
        {
            viewerDevice_->viewWarning(fh.what() + '\n');
            std::ofstream fails("failConnections.log", std::ios_base::app);
            fails << "Address: " << request_.getHost() + request_.getRelativePath()
                    << std::endl << fh.what() << std::endl;
        }
        catch(Apx::exErrorUrl &err_url)
        {
            // If failed parse link and object of Link throws exception
            // We create report consisting from address
            // And continues data handling

            std::ofstream crash(report_url, std::ios_base::app);
            crash << err_url.what() << std::endl;
            viewerDevice_->viewWarning("An unidentified url. Results recorded in the log file: " + report_url + "\n");
        }

        // if specified max depth
        if(getMaxDepth())
        {
            ++i;

            if(i < getMaxDepth())
            {
                continue;
            }
            else
            {
                break;
            }
        }
	}

    /** ############################################################# **/

    // results of crawl website
	std::ofstream external("external_links.log");
	std::ofstream internal("internal_links.log");

	for(const auto &item : externalLinks_)
    {
        external << item << std::endl;
    }

    for(const auto &item : indexedInternal_)
    {
        internal << item << std::endl;
    }

    /** ############################################################# **/


    /** check infinite crawling **/


    // for avoid double crawling
    if(infiniteCrawl_)
    {
        // save addresses of web sites which already crawled
        indexedExternal_.push_back(host_.getHost());

        if(!externalLinks_.empty())
        {
            request_.clear();
            clearQueue(CrawlerImpl::Crawler::INDEXED_IN | CrawlerImpl::Crawler::INTERNAL);

            host_ = externalLinks_.front();
            externalLinks_.pop_front();

            crawlStart();
        }
    }

    // reset user configurations after crawling
	configByDefault();
}

// second argument expect relative address with specified name file or without it
void CrawlerImpl::Crawler::addToQueue(const Apx::TagParser &parser,
                                      const Apx::Url &relativePath, Crawler::URLS urls)
{
    Apx::Url analyzeUrl;
    std::string resultAddr;

    for (const auto &tag : parser)
    {
	    try
	    {
            // check link
            analyzeUrl.parse(tag.getAttribute("href"));

            // skip undefined files
            if(!analyzeUrl.getFile().empty()
               && analyzeUrl.getTypeOfFile() != Apx::Url::TYPE_OF_FILE::EXECUTE_WEB_FILE)
            {
                continue;
            }

            if(analyzeUrl.isAbsoluteAddr())
            {
                if (!analyzeUrl.compareHost(host_)
                    && (urls == URLS::DEFAULT || urls == URLS::EXTERNAL))
                {
                    // found external link
                    if(!existsInQueues(analyzeUrl, CrawlerImpl::Crawler::EXTERNAL))
                    {
                        // temporary compare
                        // while HttpLib don't support https
                        if(analyzeUrl.getProtocol() != "https://")
                        {
                            // add subdomain if exists
                            externalLinks_.push_back(analyzeUrl.getHost());
                        }
                    }
                    else
                    {
                        continue;
                    }

                    continue;
                }
            }

            if(analyzeUrl.isAnchor())
            {
                continue;
            }

            /**
            ** WARNING
            ** There is a possibility that there will be 2 identical addresses: / and /index.php often equivalent
            **/

            /** Second part. Handling relative path. **/

            // can throw std::runtime_error
            resultAddr = convertRelativeAddr(analyzeUrl, relativePath);

            if(!existsInQueues(resultAddr)
                && (urls == URLS::INTERNAL || urls == URLS::DEFAULT))
            {
                internalLinks_.push_back(resultAddr);
            }

        }
        catch(Apx::exErrorUrl &err_url)
        {
            // If failed parse link and object of Link throws exception
            // We create report consisting from address
            // And continues data handling

            std::ofstream crash(report_url, std::ios_base::app);
            crash << err_url.what() << std::endl;
        }
    }
}

// true if found specified link in specified queue, otherwise false
bool CrawlerImpl::Crawler::existsInQueues(const Apx::Url &url, int whereToSearch)
{
    if(whereToSearch == CrawlerImpl::Crawler::INTERNAL)
    {
        for(const auto &item : indexedInternal_)
        {
            if (item == url.getRelativePath())
            {
                return true;
            }
        }

        for (const auto &item : internalLinks_)
        {
            if (item == url.getRelativePath())
            {
                return true;
            }
        }

        return false;
    }

    if(whereToSearch == CrawlerImpl::Crawler::EXTERNAL)
    {
        for (const auto &item : indexedExternal_)
        {
            // compare with full address
            if(url.compareHost(item))
            {
                return true;
            }
        }

        for (const auto &item : externalLinks_)
        {
            // compare with full address
            if(url.compareHost(item))
            {
                return true;
            }
        }
    }

	return false;
}

void CrawlerImpl::Crawler::setStartAddress(const std::string &startAddr)
{
	host_.parse(startAddr);	// <= test url

	// if link is valid
	if (host_.isAbsoluteAddr())
	{
	    if(viewerDevice_ != nullptr)
		{
		    readyForStart_ = true;
		}
	}
}

bool CrawlerImpl::Crawler::readyForStart()
{
	return readyForStart_;
}

void CrawlerImpl::Crawler::start()
{
	if (readyForStart())
	{
		crawlStart();
	}
}

void CrawlerImpl::Crawler::clearQueue(int queueMark)
{
    if((queueMark & CrawlerImpl::Crawler::INTERNAL) == CrawlerImpl::Crawler::INTERNAL)
    {
        internalLinks_.clear();
    }
    if((queueMark & CrawlerImpl::Crawler::INDEXED_IN) == CrawlerImpl::Crawler::INDEXED_IN)
    {
        indexedInternal_.clear();
    }
    if((queueMark & CrawlerImpl::Crawler::EXTERNAL) == CrawlerImpl::Crawler::EXTERNAL)
    {
        externalLinks_.clear();
    }
    if((queueMark & CrawlerImpl::Crawler::INDEXED_EX) == CrawlerImpl::Crawler::INDEXED_EX)
    {
        indexedExternal_.clear();
    }
}

void CrawlerImpl::Crawler::clearInternalQueue()
{
    clearQueue(CrawlerImpl::Crawler::INTERNAL);
}

void CrawlerImpl::Crawler::clearExternalQueue()
{
    clearQueue(CrawlerImpl::Crawler::EXTERNAL);
}

void CrawlerImpl::Crawler::clearIndexedInQueue()
{
    clearQueue(CrawlerImpl::Crawler::INDEXED_IN);
}

void CrawlerImpl::Crawler::clearIndexedExQueue()
{
    clearQueue(CrawlerImpl::Crawler::INDEXED_EX);
}

void CrawlerImpl::Crawler::configByDefault()
{
    host_.clear();
    readyForStart_ = false;
    request_.clear();
    timePause_ = STD_PAUSE_BETWEEN_REQUESTS;
    clearQueue(CrawlerImpl::Crawler::INDEXED_IN | CrawlerImpl::Crawler::INTERNAL);
}

// add link into specified queue with check on uniqueness
// if addition into internal queue then adds only relative path.
// if addition into external queue then adds url.getMainAddress()
void CrawlerImpl::Crawler::addLinkToQueue(const Apx::Url &url, int whereToSearch)
{
    if(whereToSearch == CrawlerImpl::Crawler::INTERNAL)
    {
        if(!existsInQueues(url))
        {
            internalLinks_.push_back(url.getRelativePath());
        }
        return;
    }

    if(whereToSearch == CrawlerImpl::Crawler::INDEXED_IN)
    {
        if(!existsInQueues(url))
        {
            indexedInternal_.push_back(url.getRelativePath());
        }
        return;
    }

    if(whereToSearch == CrawlerImpl::Crawler::EXTERNAL)
    {
        if(!existsInQueues(url, whereToSearch))
        {
            externalLinks_.push_back(url.getHost());
        }
        return;
    }

    if(whereToSearch == CrawlerImpl::Crawler::INDEXED_EX)
    {
        if(!existsInQueues(url, whereToSearch))
        {
            indexedExternal_.push_back(url.getHost());
        }
        return;
    }
}

// saves text of page in file
void CrawlerImpl::Crawler::savePage(const std::string &name, const std::string &text)
{
    std::string resultPath = name;
    replaceForbidChars(resultPath);
    resultPath = "index/" + host_.getHost() + "/" + resultPath;
    if(!pathToSave_.empty())
    {
        // directory by default
        resultPath = pathToSave_ + resultPath;
    }

    mkdir("./index", 0);
    mkdir(("./index/" + host_.getHost()).c_str(), 0);

    std::ofstream out(resultPath);

    if(out)
    {
        out << text;
        out.clear();
        out.close();
    }
}

std::string CrawlerImpl::Crawler::replaceChar(char ch)
{
    std::string web_equivalent;

    switch(ch)
    {
    case '\\':
        web_equivalent = "%5C";
        return web_equivalent;
    case '/':
        web_equivalent = "%2F";
        return web_equivalent;
    case ':':
        web_equivalent = "%3A";
        return web_equivalent;
    case '*':
        web_equivalent = "%2A";
        return web_equivalent;
    case '?':
        web_equivalent = "%3F";
        return web_equivalent;
    case '"':
        web_equivalent = "%22";
        return web_equivalent;
    case '<':
        web_equivalent = "%3C";
        return web_equivalent;
    case '>':
        web_equivalent = "%3E";
        return web_equivalent;
    case '|':
        web_equivalent = "%7C";
        return web_equivalent;
    default:
        return web_equivalent;
    }
}

// \/:*?"<>|
void CrawlerImpl::Crawler::replaceForbidChars(std::string &str)
{
    std::string replacer;
    for(std::size_t i = 0; i < str.size(); ++i)
    {
        if( !(replacer = replaceChar(str[i])).empty() )
        {
            str.erase(i, 1);
            str.insert(i, replacer);
            i += 2;
        }
    }
}

/** *********************** API FOR CONVERSATION TRUE RELATIVE PATHES ************************************************ **/

// converts a relative path in accordance with the place where the received address
std::string CrawlerImpl::Crawler::convertRelativeAddr(const Apx::Url &relAddr, const Apx::Url &where)
{
    // if path specified beginning slash
    // it means path specified relative by root of website
    // returns it
    if(relAddr.getRelativePath()[0] == '/')
    {
        return relAddr.getRelativePath();
    }

    if(relAddr.getRelativePath().empty() && where.getRelativeDir().empty())
    {
        // root
        return "/";
    }

    if(relAddr.getRelativePath().empty())
    {
        return where.getRelativePath();
    }

    if(where.getRelativeDir().empty())
    {
        return relAddr.getRelativePath();
    }

    /** Handling **/
    std::vector<std::string> dirFirst;
    std::vector<std::string> dirSecond;

    if(!relAddr.getRelativeDir().empty())
    {
        dirFirst = dividePath(relAddr.getRelativeDir());
    }

    dirSecond = dividePath(where.getRelativeDir());

    std::size_t sizeFirst = dirFirst.size();
    std::size_t sizeSecond = dirSecond.size();

    std::size_t counter = 0;
    std::vector<std::size_t> indexDots; // consisting indexes of elements of dots in path ./folder/

    for(std::size_t i = 0; i < sizeFirst; ++i)
    {
        if(dirFirst[i] == "..")
        {
            ++counter;
        }

        if(dirFirst[i] == ".")
        {
            indexDots.push_back(i);
        }
    }

    std::string result;

    // delete as many folders as met ".."
    for(std::size_t i = 0; i < sizeSecond && i < counter; ++i)
    {
        dirSecond.pop_back();
    }

    // resize
    sizeSecond = dirSecond.size();

    // build parts of path, where received link
    for(std::size_t i = 0; i < sizeSecond; ++i)
    {
        result += dirSecond[i] + '/';
    }

    // build link
    for(auto i = counter; i < sizeFirst; ++i)
    {
        if(!General::findValueInVector(indexDots, i))
        {
            result += dirFirst[i] + '/';
        }
    }

    // add name of file and vars
    result += relAddr.getFile() + relAddr.getVariablesString();

    return result[0] != '/' ? ('/' + result) : result;
}

// divide path by folders
// expects path like this: [/]aaa/[bbb/[page.php[?var1=val&var2=val]]]
// require last slash
std::vector<std::string> CrawlerImpl::Crawler::dividePath(const std::string &path)
{
    if(path.empty())
    {
        return std::vector<std::string>();
    }

    std::vector<std::string> dirs;
    std::string::size_type pointer = path.find('/');

    if(pointer != std::string::npos)
    {
        std::string tmpFolder;
        std::string::size_type pos = 0;

        do
        {
            tmpFolder.assign(path, pos, !pos ? pointer : pointer - pos);
            dirs.push_back(tmpFolder);
            pos = pointer + 1;
        }
        while( (pointer = path.find('/', pos)) != std::string::npos);
    }

    return dirs;
}
