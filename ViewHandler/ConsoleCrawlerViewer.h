#ifndef CONSOLE_VIEW_HANDLER_H_INCLUDED
#define CONSOLE_VIEW_HANDLER_H_INCLUDED

#include "CrawlerViewer.h"

namespace ViewHandler
{

class ConsoleCrawlerViewer
    : public CrawlerViewer
{
    virtual void viewInfo(const std::string &url,
                  size_type extl,
                  size_type intl,
                  size_type indexed) const;

    virtual void viewResponseStatus(int code) const;
    virtual void viewWarning(const std::string &warning) const;
};

} // ViewHandler

#endif // CONSOLEVIEWHANDLER_H_INCLUDED
