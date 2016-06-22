#include "Apx.h"

// getters
const std::string & Apx::Tag::getName() const
{
	return tagName_;
}

const std::string & Apx::Tag::getValue() const
{
	return tagValue_;
}

const std::deque<Apx::Attribute> & Apx::Tag::getAllAttributes() const
{
	return attributes_;
}

// returns value of attribute
// empty string if occurs error
const std::string Apx::Tag::getAttribute(const std::string &nameAttribute) const
{
	if (nameAttribute.empty())
	{
		return "";
	}

	int countAttrs = attributes_.size();
	for (int i = 0; i < countAttrs; i++)
	{
		if (strToLower(attributes_[i].getName()) == strToLower(nameAttribute))
		{
			return attributes_[i].getValue();
		}
	}

	return "";
}

// setters
void Apx::Tag::setName(const std::string & setTagName)
{
	tagName_ = setTagName;
}

void Apx::Tag::setAttributes(const std::deque<Attribute> & setAttributes)
{
	attributes_ = setAttributes;
}

void Apx::Tag::setAttribute(const std::string &attributeName, const std::string &attributeValue)
{
	Attribute tmp;
	tmp.setName(attributeName);
	tmp.setValue(attributeValue);

	attributes_.push_back(tmp);
}

void Apx::Tag::setValue(const std::string &setValueArg)
{
	tagValue_ = setValueArg;
}
