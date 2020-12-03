#pragma once
#include "InmemoryRepository.h"
#include "FileRepository.h"
#include "SqlRepository.h"
#include "DynamicArrayIterator.h"
#include "Action.h"
class Service
{
private:
	Repository* Collection;
	Repository* Mylist;
	IteratorPair* ActiveIterator;
	std::vector<std::unique_ptr<Action>> StorageUndoStack;
	std::vector<std::unique_ptr<Action>> StorageRedoStack;
	std::vector<std::unique_ptr<Action>> MylistUndoStack;
	std::vector<std::unique_ptr<Action>> MylistRedoStack;
public:
	Service(Repository* Storage = new InmemoryRepository, Repository* Mylist = new InmemoryRepository); // just so we dont edit so many tests
	~Service();

	// The commands
	void AddCommand(std::string, std::string, float, int, std::string);
	void DeleteCommand(std::string);
	void UpdateCommand(std::string, std::string, float, int, std::string);
	std::vector<OrganicFragment*>* ListCommand();
	std::vector<OrganicFragment*>* ListBySizeAndQuantityOfMicroFragmentsCommand(std::string Size, int NewOrganicFragmentQuantityOfMicroFragmentsAsInt);
	OrganicFragment* ListBySizeCommand(std::string Size);
	OrganicFragment* ListAllCommand();
	OrganicFragment* NextCommand();
	void Save(std::string ExistingOrganicFragmentId);
	std::vector<OrganicFragment*>* MyListCommand();
	void OpenMylistCommand();
	//
	void UndoStorage();
	void RedoStorage();
	void UndoMylist();
	void RedoMylist();
};