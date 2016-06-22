#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED

#include <deque>
#include <string>

namespace ViewHandler
{
class CrawlerViewer
{
public:

    using size_type = std::deque<std::string>::size_type;

    virtual void viewInfo(const std::string &url,
                  size_type extl,
                  size_type intl,
                  size_type indexed) const = 0;

    virtual void viewResponseStatus(int code) const = 0;
    virtual void viewWarning(const std::string &warning) const = 0;
};
} // ViewHandler

#endif // VIEW_H_INCLUDED
