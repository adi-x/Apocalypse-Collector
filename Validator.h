#pragma once
#include <string>
#include "Domain.h"
class Validator
{
public:
	static void StringIsInt(std::string);
	static void StringIsFloat(std::string);
	static void CanList(OrganicFragment*);
	static void ExistantInput(std::string);
};