#ifndef SAPHT_H
#define SAPHT_H

#include <string>
#include <deque>
#include <vector>

namespace Apx // API for parsing XML
{
	enum class REMOVE
	{
		ALL_TAGS = -1,
		LAST_TAG,
		FIRST_TAG
	};

	enum class SYMBOLS
	{
		EQUALLY = '=',
		OPEN_TAG = '<',
		CLOSE_TAG = '>',
		DBL_QUOTE = '\"',
		SNGL_QUOTE = '\'',
		NEW_LINE = '\n'
	};

	class Tag;
	class Attribute;

	// returns a string with lower case letters
	std::string strToLower(const std::string & toLowStr);

	// remove white-spaces left and right
	std::string cutSpaceChars(std::string &trStr);

	// cut all tags from page and return result data
	std::string cutText(const std::string &xmlPage);

	/**
    ** reads first occur tag and return it
    ** option isDoubleTag, if set to false then test pairing a tag array-based singleTags
    ** otherwise the function without checking the pairing will attempt to read the tag value, believing that it doubles
    ** this can lead to errors such as the specified tag is found, but as it is not paired,
    ** the function will try to read everything before closing this tag.
    ** In the end, is considered to be the entire page, which would be considered a tag value
    */
	Tag getNextTag(std::string &xmlPage, std::size_t &position, bool isDoubleTag = false);

	std::string removeTagFromPage(const std::string &xmlPage, const std::string &nameTag, REMOVE numberOfTag = REMOVE::FIRST_TAG);

    // Starting from the given position "position" replaces first occur specified tag name specified in "replacement"
	std::string replaceTagFromPage(std::string &xmlPage, const std::string &tag, const std::string &replacement, std::size_t &position, bool withCloseTag = false);

	// #####################################################################################################

    // may be change on std::pair
	class Attribute
	{
	public:
	    Attribute() {}
	    Attribute(const std::string &name, const std::string &value);

		// getters
		const std::string & getName() const;
		const std::string & getValue() const;

		// setters
		void setName(const std::string & setAttrName);
		void setValue(const std::string & setAttrValue);

	private:

		std::string attributeName_;
		std::string attributeValue_;

	};

	// #####################################################################################################

	class Tag
	{
	public:
		// getters
		const std::string & getName() const;
		const std::string & getValue() const;
		const std::deque<Attribute> & getAllAttributes() const;

		// returns value of attribute
        // empty string if occurs error
		const std::string getAttribute(const std::string &nameAttribute) const;

		// setters
		void setName(const std::string & setTagName);
		void setAttributes(const std::deque<Attribute> & setAttributes);
		void setAttribute(const std::string &attributeName, const std::string &attributeValue);
		void setValue(const std::string &setValueArg);

	private:

		std::string tagName_;
		std::string tagValue_;
		std::deque<Attribute> attributes_;

	};

	// #####################################################################################################

	class TagParser
	{
		// main data
		std::deque<Tag> tags_;

		bool inviteMode_;

		// clear queue of tags if turned off inviteMode
		void testInviteMode();

	public:

        using size_type = std::deque<std::string>::size_type;

		TagParser() : inviteMode_(false) {}
		TagParser(const TagParser &parser);
		TagParser &operator=(const TagParser &parser);

		// in order to support call range-for operator
        const auto begin() const -> decltype(tags_.begin()) { return tags_.begin(); }
        const auto end() const -> decltype(tags_.end()) { return tags_.end(); }

		// parse XML page by tags
		void parseByTags(const std::string &filename);

		// parse specified tags from first argument
		void parseTags(const std::string &xmlText, const std::string &nameTag, bool parseAttributes = true);

		// parse specified tags with value from first argument
		void parseTagsWithValue(const std::string &xmlText, const std::string &nameTag, bool parseAttributes = true);

		// get i tag
		const Tag &operator[](size_type i) const;

        // returns amount of read tags
		size_type count() const { return tags_.size(); }
		size_type size() const { return tags_.size(); }

		// count specified tag
		size_type countTag(const std::string &tag) const;
		void clear() { tags_.clear(); }

		// set inviteMode
		void setInviteMode(bool setInvMode) { inviteMode_ = setInvMode; }
	};

	// #####################################################################################################
}

#endif
