#include "CsvRepository.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <algorithm>

/*
void MylistCsv::Open()
{
	
}*/

CsvRepository::CsvRepository(std::string Path) : Path(Path) {}

void CsvRepository::AddFragment(OrganicFragment* SomeOrganicFragment)
{
	std::ofstream File(Path, std::ios::app);
	File << SomeOrganicFragment;
	File.close();
	delete SomeOrganicFragment;
}

void CsvRepository::DeleteFragment(std::string OrganicFragmentId)
{
	auto TemporaryStorage = Filter();
	std::function<bool(OrganicFragment*)> FilterFunction = [&OrganicFragmentId](OrganicFragment* Element)
	{
		return Element->GetId() == OrganicFragmentId;
	};
	TemporaryStorage->erase(remove_if(TemporaryStorage->begin(), TemporaryStorage->end(), FilterFunction), TemporaryStorage->end());
	std::ofstream File(Path);
	for (auto Element : *TemporaryStorage)
	{
		File << Element;
		delete Element;
	}
	File.close();
	delete TemporaryStorage;
}

void CsvRepository::UpdateFragment(std::string OrganicFragmentId, OrganicFragment* NewOrganicFragment)
{
	auto TemporaryStorage = Filter();
	std::function<bool(OrganicFragment*)> FilterFunction = [&OrganicFragmentId](OrganicFragment* Element)
	{
		return Element->GetId() == OrganicFragmentId;
	};
	replace_if(TemporaryStorage->begin(), TemporaryStorage->end(), FilterFunction, NewOrganicFragment);
	std::ofstream File(Path);
	for (auto Element : *TemporaryStorage)
	{
		File << Element;
		delete Element;
	}
	File.close();
	delete TemporaryStorage;
}

OrganicFragment* CsvRepository::Search(std::string OrganicFragmentId)
{
	auto TemporaryStorage = Filter();
	OrganicFragment* ReturnValue = NULL;
	for (auto Element : *TemporaryStorage)
		if (Element->GetId() == OrganicFragmentId)
			ReturnValue = Element;
		else delete Element;
	delete TemporaryStorage;
	return ReturnValue;
}

std::vector<OrganicFragment*>* CsvRepository::Filter(std::function<bool(OrganicFragment*)> FilterFunction)
{
	std::ifstream File(Path);
	std::vector<OrganicFragment*>* TemporaryStorage = new std::vector<OrganicFragment*>;
	OrganicFragment* StoredElement;
	while (File >> (StoredElement = new OrganicFragment{}))
		if (FilterFunction(StoredElement))
			TemporaryStorage->push_back(StoredElement);
	delete StoredElement;
	File.close();
	return TemporaryStorage;
}

void CsvRepository::Open()
{
	std::string Command = "start excel \"" + Path + "\"";
	system(Command.c_str());
}
