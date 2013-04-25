#include "DbElement.h"

DbElement::DbElement(void) : m_id(0)
{
}

DbElement::DbElement(int id) : m_id(id)
{
}

DbElement::~DbElement(void)
{
}

int DbElement::getId(void) const
{
	return this->m_id;
}

void DbElement::setId(int id)
{
	this->m_id = id;
}