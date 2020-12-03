#include "InmemoryRepository.h"
#include <algorithm>
#define FIRST_INDEX 0
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

InmemoryRepository::InmemoryRepository()
{
	RepositoryStorage = new std::vector<OrganicFragment*>;
}

InmemoryRepository::~InmemoryRepository()
{
	delete RepositoryStorage;
}

void InmemoryRepository::AddFragment(OrganicFragment* SomeOrganicFragment)
{
	RepositoryStorage->push_back(SomeOrganicFragment);
}

void InmemoryRepository::DeleteFragment(std::string OrganicFragmentId)
{
	std::function<bool(OrganicFragment*)> FilterFunction = [&OrganicFragmentId](OrganicFragment* Element)
	{
		return Element->GetId() == OrganicFragmentId;
	};
	RepositoryStorage->erase(remove_if(RepositoryStorage->begin(), RepositoryStorage->end(), FilterFunction), RepositoryStorage->end());
}

void InmemoryRepository::UpdateFragment(std::string OrganicFragmentId, OrganicFragment* NewOrganicFragment)
{
	std::function<bool(OrganicFragment*)> FilterFunction = [&OrganicFragmentId](OrganicFragment* Element)
	{
		return Element->GetId() == OrganicFragmentId;
	};
	replace_if(RepositoryStorage->begin(), RepositoryStorage->end(), FilterFunction, NewOrganicFragment);
}

OrganicFragment* InmemoryRepository::Search(std::string OrganicFragmentId)
{
	std::vector<OrganicFragment*>::iterator SoughtOrganicFragment = std::find_if(RepositoryStorage->begin(), RepositoryStorage->end(), 
		[&OrganicFragmentId](OrganicFragment* Element) 
		{
			return (Element->GetId() == OrganicFragmentId); 
		});
	if (SoughtOrganicFragment == RepositoryStorage->end())
		return NULL;
	return new OrganicFragment(*SoughtOrganicFragment);
}

std::vector<OrganicFragment*>* InmemoryRepository::Filter(std::function<bool(OrganicFragment*)> FilterFunction)
{
	std::vector<OrganicFragment*>* FilteredList = new std::vector<OrganicFragment*>;
	std::for_each(RepositoryStorage->begin(), RepositoryStorage->end(), 
		[&FilterFunction, &FilteredList](OrganicFragment* Element) 
		{
			if (FilterFunction(Element) == true)
				FilteredList->push_back(new OrganicFragment(Element));
		});
	return FilteredList;
}