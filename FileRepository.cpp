#include "FileRepository.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define FIRST_INDEX 0
#define LINE_SIZE 1000
#include "AppException.h"
// Function to delete each object from a vector
static void CleanVector(std::vector<OrganicFragment*> &SomeList)
{
	std::for_each(SomeList.begin(), SomeList.end(),
		[&SomeList](OrganicFragment* Element)
		{
			delete Element;
		});
}

// Constructor (creates the file if it doesn't exist)
OrganicFragmentFileRepository::OrganicFragmentFileRepository(std::string FileLocation)
{
	this->FileLocation = FileLocation;
	std::ofstream Dummy(FileLocation, std::ios::app);
	Dummy.close();
}


// Destructor
OrganicFragmentFileRepository::~OrganicFragmentFileRepository()
{
}


// Takes the contents of the file stored in the repo and creates a vector out of it
std::vector<OrganicFragment*> OrganicFragmentFileRepository::LoadData()
{
	std::ifstream File(FileLocation);
	std::vector<OrganicFragment*> TemporaryStorage;
	OrganicFragment* StoredElement;
	while (File >> (StoredElement = new OrganicFragment{}))
		TemporaryStorage.push_back(StoredElement);
	delete StoredElement; // the last one created, which was failed to be added.
	File.close();
	return TemporaryStorage;
}

// Takes a vector and write it's contents into the file stored in the repo
void OrganicFragmentFileRepository::OverwriteData(std::vector<OrganicFragment*> TemporaryStorage)
{
	std::ofstream File(FileLocation);
	std::for_each(TemporaryStorage.begin(), TemporaryStorage.end(),
		[&File](OrganicFragment* Element)
		{
			File << Element;
		});
	File.close();
}

// Function to add a new element in the repo (appends it to the end of the list)
void OrganicFragmentFileRepository::AddFragment(OrganicFragment* SomeOrganicFragment)
{
	std::ofstream File(FileLocation, std::ios::app);
	File << SomeOrganicFragment;
	File.close();
	delete SomeOrganicFragment;
}

// Function to delete an element from the repo
void OrganicFragmentFileRepository::DeleteFragment(std::string OrganicFragmentId)
{
	std::vector<OrganicFragment*> TemporaryStorage = LoadData();
	std::function<bool(OrganicFragment*)> FilterFunction = [&OrganicFragmentId](OrganicFragment* Element)
	{
		return Element == NULL;
	};
	std::for_each(TemporaryStorage.begin(), TemporaryStorage.end(),
		[&OrganicFragmentId](OrganicFragment* &Element)
		{
			if (Element->GetId() == OrganicFragmentId)
			{
				delete Element;
				Element = NULL;
			}
		});
	auto ElementsToDelete = remove_if(TemporaryStorage.begin(), TemporaryStorage.end(), FilterFunction);
	TemporaryStorage.erase(ElementsToDelete);
	OverwriteData(TemporaryStorage);
	CleanVector(TemporaryStorage);
}

// Function to update an element from the repo
void OrganicFragmentFileRepository::UpdateFragment(std::string OrganicFragmentId, OrganicFragment* NewOrganicFragment)
{
	std::vector<OrganicFragment*> TemporaryStorage = LoadData();
	OrganicFragment* OldFragment = NULL;
	std::function<bool(OrganicFragment*)> FilterFunction = [&OrganicFragmentId, &OldFragment](OrganicFragment* Element)
	{
		if (Element->GetId() == OrganicFragmentId)
		{
			OldFragment = Element;
			return true;
		}
		return false;
	};
	replace_if(TemporaryStorage.begin(), TemporaryStorage.end(), FilterFunction, NewOrganicFragment);
	delete OldFragment;
	OverwriteData(TemporaryStorage);
	CleanVector(TemporaryStorage);
}

// Function to search for an element in the repo
OrganicFragment* OrganicFragmentFileRepository::Search(std::string OrganicFragmentId)
{
	std::vector<OrganicFragment*> TemporaryStorage = LoadData();
	std::vector<OrganicFragment*>::iterator SoughtOrganicFragment = std::find_if(TemporaryStorage.begin(), TemporaryStorage.end(),
		[&OrganicFragmentId](OrganicFragment* Element)
		{
			return (Element->GetId() == OrganicFragmentId);
		});
	OrganicFragment* ReturnValue;
	if (SoughtOrganicFragment == TemporaryStorage.end())
		ReturnValue = NULL;
	else
		ReturnValue = new OrganicFragment(*SoughtOrganicFragment);
	CleanVector(TemporaryStorage);
	return ReturnValue;
}

// Function to filter the list from repo
std::vector<OrganicFragment*>* OrganicFragmentFileRepository::Filter(std::function<bool(OrganicFragment*)> FilterFunction)
{
	std::vector<OrganicFragment*> TemporaryStorage = LoadData();
	std::vector<OrganicFragment*>* FilteredList = new std::vector<OrganicFragment*>;
	std::for_each(TemporaryStorage.begin(), TemporaryStorage.end(),
		[&FilterFunction, &FilteredList](OrganicFragment* Element)
		{
			if (FilterFunction(Element) == true)
				FilteredList->push_back(new OrganicFragment(Element));

		});
	CleanVector(TemporaryStorage);
	return FilteredList;
}

void OrganicFragmentFileRepository::Open()
{
	std::string Command = "start \"notepad\" \"" + FileLocation + "\"";
	system(Command.c_str());
}
