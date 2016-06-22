#include <boost/regex.hpp>
#include <iostream>
#include <cstring>
#include <exception>
#include "BaseApi.h"
#include "Apx.h"

Apx::TagParser::TagParser(const TagParser &parser)
{
	inviteMode_ = parser.inviteMode_;
	clear();

	int hmTags = parser.count();
	for (int i = 0; i < hmTags; ++i)
	{
		tags_.push_back(parser[i]);
	}
}

Apx::TagParser &Apx::TagParser::operator=(const TagParser &parser)
{
	if (this == &parser)
	{
		return *this;
	}

	inviteMode_ = parser.inviteMode_;
	clear();

	size_type countTags = parser.count();
	for (size_type i = 0; i < countTags; ++i)
	{
		tags_.push_back(parser[i]);
	}

	return *this;
}

// parse page by tags
void Apx::TagParser::parseByTags(const std::string &xmlPage)
{
	testInviteMode();

	if (xmlPage.empty())
	{
		return;
	}

	std::string tag, valueOfTag;
	decltype(xmlPage.size()) lengthOfPage = xmlPage.size(), pointer = 0, mem = 0;

	while (pointer < lengthOfPage)
	{
		Tag tmpTag;
		Attribute tmpAttr;

		tag = readTag(xmlPage, pointer);
		tmpTag.setName(strToLower(getTagName(tag)));
		tmpTag.setAttributes(readAllAttributesOfTag(tag));

		if (tag[0] != '/')
		{
			if (!findInSingleTags(getTagName(tag)))
			{
				mem = pointer;
				valueOfTag = readValueToTag(xmlPage, "/" + getTagName(tag), pointer);
				tmpTag.setValue(valueOfTag);
				pointer = mem;
			}

			tags_.push_back(tmpTag);
		}
	}
}

// parse specified tags from first argument
void Apx::TagParser::parseTags(const std::string &xmlPage, const std::string &nameTag, bool parseAttributes)
{
	testInviteMode();

	if (nameTag.empty() || xmlPage.empty() || nameTag == "!--")
	{
		return;
	}

	decltype(xmlPage.size()) position = 0, size = xmlPage.size();

	// text of read the tag like this - tag [attribute1="value1" attribute2="value2"]
	std::string tagText;
	std::deque<Attribute> attributesOfNameTag;

	// name of current read tag
	std::string nameOfReadedTag, nameTagCmp = strToLower(nameTag);


	while (position < size)
	{
	    // initializing object of Tag
		Tag tmpTag;
		tagText = readTag(xmlPage, position);

        // if find the specified tag, write it
		if (strToLower(getTagName(tagText)) == nameTagCmp)
		{
			tmpTag.setName(nameTagCmp);

            // if need parse attributes
			if (parseAttributes)
			{
				tmpTag.setAttributes(readAllAttributesOfTag(tagText));
			}

			tags_.push_back(tmpTag);
		}
	}
}

// parse specified tags with value from first argument
void Apx::TagParser::parseTagsWithValue(const std::string &xmlPage, const std::string &nameTag, bool parseAttributes)
{
	testInviteMode();

	if (nameTag.empty() || xmlPage.empty() || nameTag == "!--")
	{
		return;
	}

	std::size_t position = 0, size = xmlPage.size();

	std::string tagText, nameTagCmp = strToLower(nameTag);
	std::deque<Attribute> attributesOfNameTag;

	while (position < size)
	{
		Tag tmpTag;
		tagText = readTag(xmlPage, position);

		if (strToLower(getTagName(tagText)) == nameTagCmp)
		{
			tmpTag.setName(nameTagCmp);

            // if need parse attributes of tag
			if (parseAttributes)
			{
				tmpTag.setAttributes(readAllAttributesOfTag(tagText));
			}

			tmpTag.setValue(readValueToTag(xmlPage, "/" + getTagName(tagText), position));

			tags_.push_back(tmpTag);
		}
	}
}

// get i tag
const Apx::Tag &Apx::TagParser::operator[](size_type i) const
{
	size_type countTags = tags_.size();

	if (i > countTags || !countTags)
	{
		throw std::out_of_range("Error indexing array");
	}

	return tags_[i];
}

// count specified tag
Apx::TagParser::size_type Apx::TagParser::countTag(const std::string &tag) const
{
	size_type number = count(), numberOfTagsFound = 0;

	if (number > 0)
	{
		for (size_type i = 0; i < number; ++i)
		{
			if (tags_[i].getName() == tag)
			{
				numberOfTagsFound++;
			}
		}

		return numberOfTagsFound;
	}
	else
    {
        return 0;
    }
}

// clear queue of tags if turned off inviteMode
void Apx::TagParser::testInviteMode()
{
	if (!inviteMode_)
	{
		tags_.clear();
	}
}
