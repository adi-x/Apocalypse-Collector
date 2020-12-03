#include "Validator.h"
#include "AppException.h"

void Validator::StringIsInt(std::string AnyString)
{
	for (size_t i = 0; i < AnyString.length(); i++)
		if (AnyString[i] < '0' || AnyString[i] > '9')
			throw AppException("Invalid value type!");
}

void Validator::StringIsFloat(std::string AnyString)
{
	int DotCount = 0;
	for (size_t i = 0; i < AnyString.length(); i++)
		if (AnyString[i] == '.')
			DotCount++;
		else if (AnyString[i] < '0' || AnyString[i] > '9')
			throw AppException("Invalid value type!");
	if (DotCount > 1)
		throw AppException("Invalid value type!");
}

void Validator::CanList(OrganicFragment* SomeOrganicFragment)
{
	if (SomeOrganicFragment == NULL)
		throw AppException("There are no organic fragments with that size!");
}

void Validator::ExistantInput(std::string AnyString)
{
	if (AnyString == "")
		throw AppException("Not enough parameters!");
}