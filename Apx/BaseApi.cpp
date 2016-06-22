#include <cstddef>
#include "BaseApi.h"
#include "Apx.h"

static const std::string singleTags[] =
{
    "area", "base", "basefont", "bgsound", "br", "col", "command",
    "embed", "frame", "hr", "img", "input", "isindex", "keygen",
    "source", "track", "wbr", "link", "meta", "param", "!doctype"
};

static std::size_t counterSingleTags = sizeof(singleTags) / sizeof(std::string);


// reads starting at the position "position" first occurence tag:
// <tag attribute="value">Value of tag</tag> returns string like this: [tag attribute="value"] without []
std::string readTag(const std::string &text, std::size_t &position)
{
	auto size = text.size();
	std::string tag;

	while (position < size)
	{
		// if found character of open tag
		if (isOpenTag(text[position++]))
		{
			if (skippedCommentaryTag(text, position))
			{

				// skip the iteration of cycle after ignore commentary rag
				// otherwise: <!-- some text --><sometag> - sometag cannot be read
				// because the next iteration of cycle attempt write name of tag and the result <nameTag.
				// After that function getTagName returns empty string.

				continue;
			}

			// write all characters up to the first character of close tag.
			while (position < size && !isCloseTag(text[position]))
			{
				tag += text[position++];
			}

			++position; // skip character ">"
			break;
		}
	}

	return tag;
}

/** EXPERIMENT */

// reads starting at the position "position" first occurence tag:
// <tag attribute="value">Value of tag</tag> returns string like this: [tag attribute="value"] without []
std::string readTag2(const std::string &xmlPage, std::size_t &position)
{
    std::string::size_type posOfCloseOfTag, posOfFoundTag = xmlPage.find('<', position);
    std::string tag;

    if(posOfFoundTag != std::string::npos)
    {
        if(isCommentaryTag(xmlPage, posOfFoundTag))
        {
            posOfFoundTag = xmlPage.find('<', posOfFoundTag + 1);
        }

        posOfCloseOfTag = xmlPage.find('>', posOfFoundTag);

        if(posOfCloseOfTag != std::string::npos)
        {
            tag.assign(xmlPage, posOfFoundTag + 1, posOfCloseOfTag - posOfFoundTag - 1);
            position = posOfCloseOfTag + 1;
        }
        else
        {
            tag.assign(xmlPage, posOfFoundTag + 1, std::string::npos);
            position = std::string::npos;
        }
    }

    return tag;
}


// receives string like this: [tag attribute1="value1" attribute2="value2"]
// returns value of specified attribute
std::string getAttributeOfTag(const std::string &tagText, const std::string &nameAttribute)
{
	std::string::size_type indexer;
	std::string valueOfAttr;

	if (tagText.empty() || nameAttribute.empty())
	{
		return "empty arg";
	}

	if ((indexer = issetAttributeOfTag(tagText, nameAttribute)) != std::string::npos)
	{
		int lengthOfAttr = nameAttribute.size(), endElemOfNameAttr = indexer + lengthOfAttr, sizeTagText = tagText.size();
		int pointOfExit = 0;

		// skip all the characters up to the first whitespace
		for (int i = endElemOfNameAttr; i < sizeTagText; ++i)
		{
			if (std::isspace((unsigned char)tagText[i]))
			{
				continue;
			}
			else
			{
				pointOfExit = i;
				break;
			}
		}

		// if found "=" otherwise fail
		if (isEqually(tagText[pointOfExit]))
		{
			int symbOfQuotes = 0;

			// skip all characters up to the first quotes
			for (int i = pointOfExit + 1; i < sizeTagText; ++i)
			{
				if (!isDblQuote(tagText[i]))
				{
					continue;
				}
				else
				{
					symbOfQuotes = i + 1;
					break;
				}
			}

			// if found quotes then write all after it up to the first quotes
			if (symbOfQuotes)
			{
				for (int i = symbOfQuotes; i < sizeTagText; ++i)
				{
					if (!isDblQuote(tagText[i]))
					{
						valueOfAttr += tagText[i];
					}
					else
					{
						break;
					}
				}

				return valueOfAttr;
			}
		}
		else
		{
			return "fail";
		}
	}

	return "fail";
}

// true if specified attribute found otherwise false
std::string::size_type issetAttributeOfTag(const std::string &tag, const std::string &attribute)
{
	std::string::size_type indexer;

	if (((indexer = tag.find(attribute)) != std::string::npos) && indexer >= 2 &&
		std::isspace((unsigned char)tag[indexer - 1]))
	{
		return indexer;
	}

	return std::string::npos;
}

// receives string like this: [tag [attribute1="value1" attribute2="value2"]]
std::string getTagName(const std::string &text)
{
	std::string nameTag;
	int size = text.size();

	if (text.empty())
	{
		return "fail";
	}

	for (int i = 0; i < size; ++i)
	{
		if (!std::isspace((unsigned char)text[i]))
		{
			nameTag += text[i];
		}
		else
		{
			break;
		}
	}

	return nameTag;
}

// reads all characters starting at the position "position"
// up to the first character of open/close any occurence tag
std::string readValue(const std::string &text, std::size_t &position)
{
	std::string value;
	auto size = text.size();

	while (position < size)
	{
		if (!isOpenTag(text[position]))
		{
			value += text[position++];
		}
		else
		{
			break;
		}
	}

	return value;
}

// reads all characters starting at the position "position" up to the specified tag
// returns all read characters, pointer moves after tag, before which should be to read
std::string readValueToTag(const std::string &text, const std::string &tag, std::size_t &position)
{
	std::string value = readValue(text, position);
	std::string whatTag = Apx::strToLower(readTag(text, position));

	auto size = text.size();

	while (position < size && getTagName(whatTag) != Apx::strToLower(tag))
	{
		whatTag = char(Apx::SYMBOLS::OPEN_TAG) + whatTag + char(Apx::SYMBOLS::CLOSE_TAG);
		value += whatTag;

		value += readValue(text, position);
		whatTag = Apx::strToLower(readTag(text, position));
	}

	return value;
}

// reads all attributes of passed tag and returns vector of attributes
// expects string like this: tag [attribute1="value1" attribute2="value2"]
// attributes can be any number
std::deque<Apx::Attribute> readAllAttributesOfTag(const std::string &tag_text)
{
    // auto = std::string::size_type
	auto size = tag_text.size();
	std::string tmpObj;

	std::deque<Apx::Attribute> tmpAttrsObj;
	Apx::Attribute tmpAttr;

	// if exists attributes, read it
	if (issetAttributesOfTag(tag_text))
	{
		tmpObj = getTagName(tag_text);
		auto lengthNameOfTag = tmpObj.size();
		tmpObj.clear();

		for (auto i = lengthNameOfTag; i < size; ++i)
		{
			if (std::isalpha((unsigned char)tag_text[i]))
			{
				// reads name of attribute before first meet character white-space or character equal
				while ((i < size) && !std::isspace((unsigned char)tag_text[i]) && !isEqually(tag_text[i]))
				{
					tmpObj += tag_text[i];
					++i;
				}

				// skip white-spaced
				while ((i < size) && std::isspace((unsigned char)tag_text[i]))
				{
					++i;
				}

				if (!isEqually(tag_text[i]))
				{
					// if after skip white-spaces meet letter
					// reduce the item to 1. Because next step of cycle "for" increase it to 1
					// This will lead to a loss of character
					tmpObj.clear();
					--i;
					continue;
				}

				tmpAttr.setName(tmpObj); // write name of attribute
				tmpObj.clear();

                bool dblQoutes = false, alnumCh = false;

				// until meet double/single quotes or letter/digit character
				while ((i < size) && !isDblQuote(tag_text[i]) && !isSnglQuote(tag_text[i])
                                    && !std::isalnum((unsigned char)tag_text[i]))
				{
					++i;
				}

                // if meet double quotes, set specifier flag
				dblQoutes = isDblQuote(tag_text[i]);
				alnumCh = std::isalnum((unsigned char)tag_text[i]);

				++i; // in order to skip character of quotes

				// If the attribute opened was double quotes
                // Write to all double quotes, or if find the letter/digit
                // Write all before the first counter white-space
                // Otherwise write as long as meet single quotes
				while
                (
                (i < size) &&
                (dblQoutes ? !isDblQuote(tag_text[i]) :
                alnumCh ? !std::isspace((unsigned char)tag_text[i]) :
                !isSnglQuote(tag_text[i]))
                )

				{
					tmpObj += tag_text[i];
					++i;
				}

				tmpAttr.setValue(tmpObj);
				tmpObj.clear();

				// add new attr in vec
				tmpAttrsObj.push_back(tmpAttr);
			}
		}
	}

	return tmpAttrsObj;
}

// true if the attributes found at the tag, otherwise false
bool issetAttributesOfTag(const std::string &tagText)
{
	std::string::size_type indexer = tagText.find((char)Apx::SYMBOLS::EQUALLY);
	decltype(indexer) i = 0;

	if (indexer != std::string::npos)
	{
		i = indexer - 1;

		if (indexer <= 2)
		{
			return false;
		}

		// skip all white-spaces between character = and last letter of name attribute
		do
		{
			--i;
		} while (std::isspace((unsigned char)tagText[i]) && i > 0);

		// skip name of attribute until meet white-space characters
		do
		{
			--i;
		} while (!std::isspace((unsigned char)tagText[i]) && i > 0);

		// test second part after character =
		if (std::isspace((unsigned char)tagText[i]))
		{
			i = indexer + 1;

			// if after = meet " or ' or letter/digit, return true
			if (isDblQuote(tagText[i]) || isSnglQuote(tagText[i]) || std::isalnum((unsigned char)tagText[i]))
			{
				return true;
			}

            // skip white-spaces
			do
			{
				++i;
			} while (std::isspace((unsigned char)tagText[i]) && i > 0);

            // if find double or single quotes or letter/digit, return true
			if (isDblQuote(tagText[i]) || isSnglQuote(tagText[i]) || std::isalnum((unsigned char)tagText[i]))
			{
				return true;
			}
			// otherwise tag invalid
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}

// moves pointer after commentary tag
bool skippedCommentaryTag(const std::string &xmlPage, std::size_t &position)
{
	auto size = xmlPage.size();
	std::string isEndComTag;

	if (isCommentaryTag(xmlPage, position))
	{
		position += 3; // shifts ptr of the open tag character of commentary tag

		while (position < size)
		{
			// if find character '-', write it and two next characters
			if (xmlPage[position] == '-')
			{
				isEndComTag = xmlPage[position];
				isEndComTag += xmlPage[position + 1];
				isEndComTag += xmlPage[position + 2];

				// shifts ptr of the character close tag and return result
				if (isEndComTag == "-->")
				{
					position += 3;
					return true;
				}
				else
				{
					isEndComTag.clear();
				}
			}

			++position;
		}
	}

	return false;
}

// true if in the text, starting from the position "position" meet the characters !--
bool isCommentaryTag(const std::string &xmlPage, std::size_t &position)
{
	if (position > xmlPage.size() || xmlPage.empty())
	{
		return false;
	}

	if (xmlPage[position] == '!' && xmlPage[position + 1] == '-' && xmlPage[position + 2] == '-')
	{
		return true;
	}

	return false;
}

// find specified the single tag. True if the tag find, otherwise false.
bool findInSingleTags(const std::string &find)
{
	for (std::size_t i = 0; i < counterSingleTags; i++)
	{
		if (Apx::strToLower(find) == singleTags[i])
		{
			return true;
		}
	}

	return false;
}

// check characters

// matches '\"'
bool isDblQuote(char ch)
{
	if (ch == (char)Apx::SYMBOLS::DBL_QUOTE)
		return true;
	return false;
}

// matches '\''
bool isSnglQuote(char ch)
{
    if (ch == (char)Apx::SYMBOLS::SNGL_QUOTE)
		return true;
	return false;
}

// matches '='
bool isEqually(char ch)
{
	if (ch == (char)Apx::SYMBOLS::EQUALLY)
		return true;
	return false;
}

// matches '<'
bool isOpenTag(char ch)
{
	if (ch == (char)Apx::SYMBOLS::OPEN_TAG)
		return true;
	return false;
}

// matches '>'
bool isCloseTag(char ch)
{
	if (ch == (char)Apx::SYMBOLS::CLOSE_TAG)
		return true;
	return false;
}

// matches '\n'
bool isNewLine(char ch)
{
	if (ch == (char)Apx::SYMBOLS::NEW_LINE)
		return true;
	return false;
}
