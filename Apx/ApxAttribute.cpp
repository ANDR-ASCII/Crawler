#include "Apx.h"

Apx::Attribute::Attribute(const std::string &name, const std::string &value)
    : attributeName_(name)
    , attributeValue_(value)
    {}

// getters
const std::string & Apx::Attribute::getName() const
{
	return attributeName_;
}

const std::string & Apx::Attribute::getValue() const
{
	return attributeValue_;
}

// setters
void Apx::Attribute::setName(const std::string & setAttrName)
{
	attributeName_ = setAttrName;
}

void Apx::Attribute::setValue(const std::string & setAttrValue)
{
	attributeValue_ = setAttrValue;
}
