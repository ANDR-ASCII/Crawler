#include <iostream>
#include <string>
#include "GeneralHelperFunctions.h"
#include "HttpLib/HttpConnection.h"
#include "Crawler.h"
#include "Menu.h"

int main()
{
    try
    {
        UserInterface::Menu menu;
    }
    catch(const std::exception &e)
    {
        std::cout << e.what();
    }

	return 0;
}
