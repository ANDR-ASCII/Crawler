#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Apx.h"
#include "BaseApi.h"

// remove white-spaces left and right
std::string Apx::cutSpaceChars(std::string &workStr)
{
    if(workStr.empty())
    {
        return workStr;
    }

    std::string temporaryStr;
    auto sizeOfStr = workStr.size();

    // particular case
    if(sizeOfStr == 1)
    {
        if(workStr != " ")
        {
            return workStr;
        }
        else
        {
            workStr.clear();
            return workStr;
        }
    }

    decltype(sizeOfStr) lblBegin = 0, lblEnd = 0;

    // set label of beginning string
    for(decltype(sizeOfStr) i = 0; i < sizeOfStr; ++i)
    {
        if(isspace((unsigned char)workStr[i]))
        {
            continue;
        }
        else
        {
            lblBegin = i;
            break;
        }
    }

    // set label of end string after last character
    for(auto i = sizeOfStr - 1; i; --i)
    {
        if(isspace((unsigned char)workStr[i]))
        {
            continue;
        }
        else
        {
            lblEnd = i + 1;
            break;
        }
    }

    if(lblBegin < lblEnd)
    {
        for(auto i = lblBegin; i != lblEnd; ++i)
        {
            temporaryStr += workStr[i];
        }
    }

    workStr = temporaryStr;
    return workStr;
}

// cut all tags from page and return result data
std::string Apx::cutText(const std::string &xmlPage)
{
	if (xmlPage.empty())
	{
		return "";
	}

	auto size = xmlPage.size();
	std::string betweenValue, value, tag;

	for (std::size_t i = 0; i < size;)
	{
		tag = strToLower(getTagName(readTag(xmlPage, i))); // continue tag
		if (tag == "script" || tag == "style")
		{
			readValueToTag(xmlPage, "/" + tag, i);
			continue;
		}

        betweenValue = readValue(xmlPage, i);
        betweenValue = cutSpaceChars(betweenValue);
		if (!betweenValue.empty()) value += betweenValue + (char)SYMBOLS::NEW_LINE;
		{
		    betweenValue.clear();
		}
	}

	return value;
}

// returns a string with lower case letters
std::string Apx::strToLower(const std::string & toLowStr)
{
	std::string resultStr;
	int size = toLowStr.size();

	for (int i = 0; i < size; ++i)
	{
		resultStr += static_cast<char>(tolower((unsigned char)toLowStr[i]));
	}

	return resultStr;
}

/**
** Reads first occur tag and return it.
** Option isDoubleTag, if set to false then test pairing a tag array-based singleTags,
** otherwise the function without checking the pairing will attempt to read the tag value, believing that it doubles.
**
** This can lead to errors such as the specified tag is found, but as it is not paired,
** the function will try to read everything before closing this tag.
** In the end, is considered to be the entire page, which would be considered a tag value.
*/
Apx::Tag Apx::getNextTag(std::string &xmlPage, std::size_t &position, bool isDoubleTag)
{
	auto size = xmlPage.size();

	if (xmlPage.empty() || position > size)
	{
		Tag retTag;
		retTag.setName("");
	}

	Tag resTag;

	std::string tmpTag = readTag(xmlPage, position);
	std::string tmpTagName = getTagName(tmpTag);

	resTag.setName(tmpTagName);
	resTag.setAttributes(readAllAttributesOfTag(tmpTag));

	if (!isDoubleTag)
	{
		if (!findInSingleTags(tmpTagName))
		{
			resTag.setValue(readValueToTag(xmlPage, "/" + tmpTagName, position));
		}
	}
	else
	{
		resTag.setValue(readValueToTag(xmlPage, "/" + tmpTagName, position));
	}

	return resTag;
}

// Starting from the given position "position" replaces first occur specified tag name specified in "replacement"
std::string Apx::replaceTagFromPage(std::string &xmlPage, const std::string &tag, const std::string &replacement, std::size_t &position, bool withCloseTag)
{
    auto sizePage = xmlPage.size();
    std::string tagText, valueOfTag;
    decltype(sizePage) markOfBegTag = 0;

    while(position < sizePage)
    {
        tagText = readTag(xmlPage, position);

        if(Apx::strToLower(getTagName(tagText)) == Apx::strToLower(tag))
        {
            // moves pointer to the first character of tag
            markOfBegTag = position - tagText.size() - 1;

            // replace an existing tag to the specified
            xmlPage.replace(markOfBegTag, getTagName(tagText).size(), replacement);

            if(withCloseTag)
            {
                // after replace tags check whether to continue to write
                if(position < xmlPage.size())
                {
                    // subtract difference between the size of the tags
                    position -= getTagName(tagText).size() - replacement.size();

                    // write the contents of the tag
                    valueOfTag = readValueToTag(xmlPage, '/' + tag, position);

                    // if the position is valid
                    if(position < xmlPage.size())
                    {
                        // label of close tag (+2 with '>' and '/')
                        position -= tagText.size() + 2;

                        // replace existing tag by the specified tag
                        xmlPage.replace(position, getTagName(tagText).size() + 1, '/' + replacement);

                        // moves pointer on the next character after character '>'
                        position += replacement.size() + 2;
                    }
                }
            }

            break;
        }
    }

    return valueOfTag;
}

// deletes specified tag by specified number from xml page
// by default number of tag: FIRST_TAG, deletes first occurence specified tag
std::string Apx::removeTagFromPage(const std::string &xmlPage, const std::string &nameTag, REMOVE numberOfTag)
{
	std::string resultPage;

	if (numberOfTag == REMOVE::ALL_TAGS)
	{
		auto lengthOfPage = xmlPage.size();

		for (decltype(lengthOfPage) position = 0; position < lengthOfPage; ++position)
		{
			if (getTagName(readTag(xmlPage, position)) == nameTag)
			{

			}
		}
	}
	return resultPage;
}
