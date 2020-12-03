#include "Service.h"
#include "AppException.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// Constructor
Service::Service(Repository* Storage, Repository* Mylist) : Collection(Storage), Mylist(Mylist), ActiveIterator(NULL) { }

// Destructor
Service::~Service()
{
	delete Collection;
	delete Mylist;
	if (ActiveIterator != NULL)
		delete ActiveIterator;
}

// Handling each of the commands
void Service::AddCommand(std::string NewOrganicFragmentId, std::string NewOrganicFragmentSize, float NewOrganicFragmentInfectionLevel, int NewOrganicFragmentQuantityOfMicroFragments, std::string NewOrganicFragmentPhotograph)
{
	// Basic Validation
	OrganicFragment* SearchResult = Collection->Search(NewOrganicFragmentId);
	if (SearchResult != NULL)
	{
		delete SearchResult;
		throw AppException("There already exists an organic fragment with that Id.");
	}
	OrganicFragment* NewOrganicFragment = new OrganicFragment(NewOrganicFragmentId, NewOrganicFragmentSize, NewOrganicFragmentInfectionLevel, NewOrganicFragmentQuantityOfMicroFragments, NewOrganicFragmentPhotograph);
	OrganicFragment* NewFragmentCopy = new OrganicFragment(NewOrganicFragment);
	Collection->AddFragment(NewOrganicFragment);
	auto Action = std::make_unique<AddAction>(NewFragmentCopy, *Collection);
	StorageUndoStack.push_back(std::move(Action));
	StorageRedoStack.empty();
	delete NewFragmentCopy;
}

void Service::DeleteCommand(std::string ExistingOrganicFragmentId)
{
	OrganicFragment* SearchResult = Collection->Search(ExistingOrganicFragmentId);
	if (SearchResult == NULL)
		throw AppException("There doesn't exist an organic fragment with that Id.");
	auto Action = std::make_unique<RemoveAction>(SearchResult, *Collection);
	StorageUndoStack.push_back(std::move(Action));
	StorageRedoStack.empty();
	Collection->DeleteFragment(ExistingOrganicFragmentId);
	delete SearchResult;
	
}

void Service::UpdateCommand(std::string OldOrganicFragmentId, std::string NewOrganicFragmentSize, float NewOrganicFragmentInfectionLevel, int NewOrganicFragmentQuantityOfMicroFragments, std::string NewOrganicFragmentPhotograph)
{
	// Basic Validation
	OrganicFragment* SearchResult = Collection->Search(OldOrganicFragmentId);
	if (SearchResult == NULL)
		throw AppException("There doesn't exist an organic fragment with that Id.");
	OrganicFragment* NewOrganicFragment = new OrganicFragment(OldOrganicFragmentId, NewOrganicFragmentSize, NewOrganicFragmentInfectionLevel, NewOrganicFragmentQuantityOfMicroFragments, NewOrganicFragmentPhotograph);
	auto Action = std::make_unique<UpdateAction>(SearchResult, NewOrganicFragment, *Collection);
	StorageUndoStack.push_back(std::move(Action));
	StorageRedoStack.empty();
	delete SearchResult;
	Collection->UpdateFragment(OldOrganicFragmentId, NewOrganicFragment);
}

std::vector<OrganicFragment*>* Service::ListCommand()
{
	return Collection->Filter();
}

std::vector<OrganicFragment*>* Service::ListBySizeAndQuantityOfMicroFragmentsCommand(std::string Size, int QuantityOfMicroFragments)
{
	return Collection->Filter([&Size, &QuantityOfMicroFragments] (OrganicFragment* SomeFragment) // using a lambda function as a parameter :D
		{ 
			if (SomeFragment->GetSize() == Size && SomeFragment->GetQuantityOfMicroFragments() >= QuantityOfMicroFragments) 
				return true; 
			return false; 
		});
}

OrganicFragment* Service::ListBySizeCommand(std::string Size)
{
	std::vector<OrganicFragment*>* FilteredList = Collection->Filter([&Size](OrganicFragment* SomeFragment) // using a lambda function as a parameter :D
		{
			if (SomeFragment->GetSize() == Size)
				return true;
			return false;
		});
	if (ActiveIterator != NULL)
		delete ActiveIterator;
	ActiveIterator = new IteratorPair(FilteredList);
	return ActiveIterator->GetCurrent();
}

OrganicFragment* Service::ListAllCommand()
{
	std::vector<OrganicFragment*>* FilteredList = Collection->Filter();
	if (ActiveIterator != NULL)
		delete ActiveIterator;
	ActiveIterator = new IteratorPair(FilteredList);
	return ActiveIterator->GetCurrent();
}

OrganicFragment* Service::NextCommand()
{
	if (ActiveIterator == NULL || ActiveIterator->GetCurrent() == NULL)
		return ListAllCommand();
	ActiveIterator->Next();
	return ActiveIterator->GetCurrent();
}

void Service::Save(std::string ExistingOrganicFragmentId)
{
	OrganicFragment* SearchResult = Collection->Search(ExistingOrganicFragmentId);
	if (SearchResult == NULL)
		throw AppException("There doesn't exist an organic fragment with that Id.");
	delete SearchResult;
	SearchResult = Mylist->Search(ExistingOrganicFragmentId);
	if (SearchResult != NULL)
	{
		delete SearchResult;
		throw AppException("The user already has that organic fragment.");
	}
	Mylist->AddFragment(Collection->Search(ExistingOrganicFragmentId));
	auto Action = std::make_unique<AddAction>(Collection->Search(ExistingOrganicFragmentId), *Mylist);
	MylistUndoStack.push_back(std::move(Action));
	MylistRedoStack.empty();
}

std::vector<OrganicFragment*>* Service::MyListCommand()
{
	return Collection->Filter();
}

void Service::OpenMylistCommand()
{
	//OrganicFragmentCollection->OpenMylist();
	Mylist->Open();
}

void Service::UndoStorage()
{
	if (StorageUndoStack.size() > 0)
	{
		auto Action = std::move(StorageUndoStack.back());
		Action->Undo();
		StorageRedoStack.push_back(std::move(Action));
		StorageUndoStack.pop_back();
	}
	else throw AppException("Nothing to undo.");
}

void Service::RedoStorage()
{
	if (StorageRedoStack.size() > 0)
	{
		auto Action = std::move(StorageRedoStack.back());
		Action->Redo();
		StorageUndoStack.push_back(std::move(Action));
		StorageRedoStack.pop_back();
	}
	else throw AppException("Nothing to redo.");
}

void Service::UndoMylist()
{
	if (MylistUndoStack.size() > 0)
	{
		auto Action = std::move(MylistUndoStack.back());
		Action->Undo();
		MylistRedoStack.push_back(std::move(Action));
		MylistUndoStack.pop_back();
	}
	else throw AppException("Nothing to undo.");
}

void Service::RedoMylist()
{
	if (MylistRedoStack.size() > 0)
	{
		auto Action = std::move(MylistRedoStack.back());
		Action->Redo();
		MylistUndoStack.push_back(std::move(Action));
		MylistRedoStack.pop_back();
	}
	else throw AppException("Nothing to redo.");
}

