#ifndef BASE_API_H
#define BASE_API_H

#include <string>
#include "Apx.h"

// functions for check characters
bool isDblQuote(char ch);
bool isSnglQuote(char ch);
bool isEqually(char ch);
bool isOpenTag(char ch);
bool isCloseTag(char ch);
bool isNewLine(char ch);


// moves the pointer to the comment tag
bool skippedCommentaryTag(const std::string &text, std::size_t &position);


// true if the text starting at position "position" occurs characters !--
bool isCommentaryTag(const std::string &text, std::size_t &position);


// reads starting at position "position" first occur tag
// <tag attr="val">value of tag</tag> returns string following: [tag attribute="value"] without "[" and ]""
// pointer "position" moves to the next character by character ">"
std::string readTag(const std::string &text, std::size_t &position);


// reads all characters starting at the position "position"
// up to the first character of open/close any occurence tag
// pointer sets on the first occurence character '<'
std::string readValue(const std::string &text, std::size_t &position);


// reads all characters starting at the position "position" up to the specified tag
// returns all read characters, pointer moves after tag, before which should be to read
std::string readValueToTag(const std::string &text, const std::string &tag, std::size_t &position);


// receives string like this: [tag attribute1="value1" attribute2="value2"]
// returns value of specified attribute
std::string getAttributeOfTag(const std::string &text, const std::string &attribute);


// receives string like this: [tagname [attribute1="value1" attribute2="value2"]]
// return tagname
std::string getTagName(const std::string &text);


// true if specified attribute found otherwise false
std::string::size_type issetAttributeOfTag(const std::string &text, const std::string &attribute);


// reads all attributes of passed tag and returns vector of attributes
// expects string like this: tag [attribute1="value1" attribute2="value2"]
// attributes can be any number
std::deque<Apx::Attribute> readAllAttributesOfTag(const std::string &tag_text);


// true if the attributes found at the tag, otherwise false
bool issetAttributesOfTag(const std::string &text);


// find specified the single tag. True if the tag find, otherwise false.
bool findInSingleTags(const std::string &find);

#endif
