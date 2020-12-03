#pragma once
#pragma once
#include "Domain.h"
#include <functional>
#include <vector>

class Repository 
{
public:
	virtual ~Repository() = 0 {};

	// Setters
	virtual void AddFragment(OrganicFragment* SomeOrganicFragment) = 0;
	virtual void DeleteFragment(std::string OrganicFragmentId) = 0;
	virtual void UpdateFragment(std::string OrganicFragmentId, OrganicFragment* NewOrganicFragment) = 0;

	// Getters
	virtual OrganicFragment* Search(std::string OrganicFragmentId) = 0;
	virtual std::vector<OrganicFragment*>* Filter(std::function<bool(OrganicFragment*)> = [](OrganicFragment*) {return true; }) = 0; // Initial declaration using a lambda function
	virtual void Open() = 0;
};