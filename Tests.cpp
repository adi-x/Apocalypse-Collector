#include "Tests.h"
#include "Service.h"
#include "AppException.h"
#include "assert.h"
#include <fstream>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <algorithm>
#include "sqlite3.h"


// Function to delete each object from a vector
static void CleanVector(std::vector<OrganicFragment*>& SomeList)
{
	std::for_each(SomeList.begin(), SomeList.end(),
		[&SomeList](OrganicFragment* Element)
		{
			delete Element;
		});
}
void ClearTestFile()
{
	{
		std::ofstream File("TestData.txt");
		File << "";
		File.close();
	}
	{
		std::ofstream File("TestData.csv");
		File << "";
		File.close();
	}
	{
		std::ofstream File("TestData.html");
		File << "";
		File.close();
	}
	{
		sqlite3* Database;
		int OperationSucces;
		OperationSucces = sqlite3_open("TestData.db", &Database);
		if (OperationSucces != 0)
			throw AppException("Could not open database");
		std::string SqlToCreateTable = "CREATE TABLE IF NOT EXISTS OrganicFragments(Id text, Size text, InfectionLevel real, QuantityOfMicroFragments int, Photograph text)";
		char* SqlErrorMessage;
		OperationSucces = sqlite3_exec(Database, SqlToCreateTable.c_str(), NULL, 0, &SqlErrorMessage);
		if (OperationSucces != SQLITE_OK)
		{
			sqlite3_free(SqlErrorMessage);
			sqlite3_close(Database);
			throw AppException("Error creating table!");
		}
		std::string SqlToEmptyTable = "DELETE FROM OrganicFragments";
		OperationSucces = sqlite3_exec(Database, SqlToEmptyTable.c_str(), NULL, 0, &SqlErrorMessage);
		if (OperationSucces != SQLITE_OK)
		{
			sqlite3_free(SqlErrorMessage);
			sqlite3_close(Database);
			throw AppException("Error emptying table!");
		}
		sqlite3_close(Database);
	}
	
}
// Add
void AddCommand_DuplicateInput_ThrowException()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("id", "a", 1, 2, "b");
		Controller->AddCommand("id", "a", 1, 2, "b");
	}
	catch (AppException)
	{
		AssertionResult = true;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void AddCommand_ValidInput_CorrectAddition()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("id", "a", 1, 2, "b");
		auto TemporaryStorage = Controller->ListCommand();
		AssertionResult = (*TemporaryStorage)[0]->GetId() == "id";
		CleanVector(*TemporaryStorage);
		delete TemporaryStorage;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void AddCommand_MassiveValidInput_CorrectResize()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		for (int i = 0; i < 120; i++)
		{
			Controller->AddCommand(std::to_string(i), "a", 1, 2, "b");
		}
		auto TemporaryStorage = Controller->ListCommand();
		AssertionResult = TemporaryStorage->size() == 120;
		CleanVector(*TemporaryStorage);
		delete TemporaryStorage;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

// Save (Add)
void SaveCommand_ValidInput_CorrectSave()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("id", "a", 1, 2, "b");
		Controller->Save("id");
		auto TemporaryStorage = Controller->MyListCommand();
		AssertionResult = (*TemporaryStorage)[0]->GetId() == "id";
		CleanVector(*TemporaryStorage);
		delete TemporaryStorage;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void SaveCommand_BadInput_ThrowException()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->Save("id");
	}
	catch (AppException)
	{
		AssertionResult = true;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void SaveCommand_DuplicateInput_ThrowException()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("id", "a", 1, 2, "b");
		Controller->Save("id");
		Controller->Save("id");
	}
	catch (AppException)
	{
		AssertionResult = true;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

// Delete
void DeleteCommand_BadInput_ThrowException()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->DeleteCommand("id");
	}
	catch (AppException)
	{
		AssertionResult = true;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void DeleteCommand_ValidInput_CorrectRemoval()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("id", "a", 1, 2, "b");
		Controller->DeleteCommand("id");
		auto TemporaryStorage = Controller->ListCommand();
		AssertionResult = TemporaryStorage->size() == 0;
		CleanVector(*TemporaryStorage);
		delete TemporaryStorage;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}


void DeleteCommand_ValidInput_CorrectRemovalFromSaved()
{
	return;
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("id", "a", 1, 2, "b");
		Controller->Save("id");
		Controller->DeleteCommand("id");
		auto TemporaryStorage = Controller->MyListCommand();
		AssertionResult = TemporaryStorage->size() == 0;
		CleanVector(*TemporaryStorage);
		delete TemporaryStorage;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}



// Update
void UpdateCommand_BadInput_ThrowException()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->UpdateCommand("id1", "b", 2, 1, "a");
	}
	catch (AppException)
	{
		AssertionResult = true;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}


void UpdateCommand_ValidInput_CorrectUpdate()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("id", "a", 1, 2, "b");
		Controller->UpdateCommand("id", "b", 1, 2, "c");
		auto TemporaryStorage = Controller->ListCommand();
		OrganicFragment* Stored = (*TemporaryStorage)[0];
		AssertionResult = Stored->GetSize() == "b" && Stored->GetInfectionLevel() == 1 && Stored->GetPhotograph() == "c" && Stored->GetQuantityOfMicroFragments() == 2;
		CleanVector(*TemporaryStorage);
		delete TemporaryStorage;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void UpdateCommand_ValidInput_CorrectUpdate_MyList()
{
	return;
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("id", "a", 1, 2, "b");
		Controller->Save("id");
		Controller->UpdateCommand("id", "b", 1, 2, "c");
		auto TemporaryStorage = Controller->ListCommand();
		OrganicFragment* Stored = (*TemporaryStorage)[0];
		AssertionResult = Stored->GetSize() == "b" && Stored->GetInfectionLevel() == 1 && Stored->GetPhotograph() == "c" && Stored->GetQuantityOfMicroFragments() == 2;
		CleanVector(*TemporaryStorage);
		delete TemporaryStorage;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}


void ListCommand_MassiveValidInput_CorrectExecution()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		for (int i = 0; i < 120; i++)
		{
			Controller->AddCommand(std::to_string(i), "a", 1, 2, "b");
		}
		std::vector<OrganicFragment*>* StoredList = Controller->ListCommand();
		OrganicFragment* SomeFragment;
		for (int i = 0; i < 120; i++)
		{
			SomeFragment = (*StoredList)[i];
			if (SomeFragment->GetId() != std::to_string(i))
				throw "Error";
		}
		AssertionResult = StoredList->size() == 120;
		CleanVector(*StoredList);
		delete StoredList;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void ListBySizeAndQuantityOfMicroFragmentsCommand_ValidInput_CorrectExecution()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("1", "1", 1, 1, "1");
		Controller->AddCommand("2", "2", 2, 2, "2");
		Controller->AddCommand("3", "3", 3, 3, "3");
		Controller->AddCommand("4", "3", 4, 4, "4");
		Controller->AddCommand("5", "3", 5, 5, "5");
		std::vector<OrganicFragment*>* StoredList = Controller->ListBySizeAndQuantityOfMicroFragmentsCommand("3", 4);
		AssertionResult = StoredList->size() == 2;
		CleanVector(*StoredList);
		delete StoredList;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void ListBySizeCommand_ValidInput_CorrectExecution()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("1", "1", 1, 1, "1");
		Controller->AddCommand("2", "2", 2, 2, "2");
		Controller->AddCommand("3", "3", 3, 3, "3");
		Controller->AddCommand("4", "3", 4, 4, "4");
		Controller->AddCommand("5", "3", 5, 5, "5");
		OrganicFragment* First = Controller->ListBySizeCommand("3");
		AssertionResult = First->GetId() == "3";
		delete First;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void ListBySizeCommand_ValidInput_DeleteOldIterator()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("1", "1", 1, 1, "1");
		Controller->AddCommand("2", "2", 2, 2, "2");
		Controller->AddCommand("3", "3", 3, 3, "3");
		Controller->AddCommand("4", "3", 4, 4, "4");
		Controller->AddCommand("5", "3", 5, 5, "5");
		OrganicFragment* First = Controller->ListBySizeCommand("3");
		delete First;
		First = Controller->ListBySizeCommand("2");
		AssertionResult = First->GetId() == "2";
		delete First;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}


void ListAll_ValidInput_CorrectExecution()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("1", "1", 1, 1, "1");
		Controller->AddCommand("2", "2", 2, 2, "2");
		Controller->AddCommand("3", "3", 3, 3, "3");
		Controller->AddCommand("4", "3", 4, 4, "4");
		Controller->AddCommand("5", "3", 5, 5, "5");
		OrganicFragment* First = Controller->ListAllCommand();
		AssertionResult = First->GetId() == "1";
		delete First;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void ListAll_ValidInput_DeleteOldIterator()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("1", "1", 1, 1, "1");
		Controller->AddCommand("2", "2", 2, 2, "2");
		Controller->AddCommand("3", "3", 3, 3, "3");
		Controller->AddCommand("4", "3", 4, 4, "4");
		Controller->AddCommand("5", "3", 5, 5, "5");
		OrganicFragment* First = Controller->ListAllCommand();
		delete First;
		delete Controller->NextCommand();
		First = Controller->ListAllCommand();
		AssertionResult = First->GetId() == "1";
		delete First;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void ListAll_EmptyList_CorrectExecution()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		OrganicFragment* First = Controller->ListAllCommand();
		AssertionResult = First == NULL;
		delete First;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void NextCommand_ValidInput_CorrectExecution()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("1", "1", 1, 1, "1");
		Controller->AddCommand("2", "2", 2, 2, "2");
		Controller->AddCommand("3", "3", 3, 3, "3");
		Controller->AddCommand("4", "3", 4, 4, "4");
		Controller->AddCommand("5", "3", 5, 5, "5");
		OrganicFragment* First = Controller->ListBySizeCommand("3");
		delete First;
		First = Controller->NextCommand();
		AssertionResult = First->GetId() == "4";
		delete First;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void NextCommand_ValidInput_CorrectCycle()
{
	ClearTestFile();
	Service* Controller = new Service;
	bool AssertionResult = false;
	try {
		Controller->AddCommand("1", "1", 1, 1, "1");
		OrganicFragment* First = Controller->ListAllCommand();
		delete First;
		First = Controller->NextCommand();
		delete First;
		First = Controller->NextCommand();
		AssertionResult = First->GetId() == "1";
		delete First;
	}
	catch (...)
	{
		// Nothing.
	}
	delete Controller;
	assert(AssertionResult);
}

void RunTests()
{
	AddCommand_ValidInput_CorrectAddition();
	AddCommand_DuplicateInput_ThrowException();
	//AddCommand_MassiveValidInput_CorrectResize();
	SaveCommand_BadInput_ThrowException();
	SaveCommand_ValidInput_CorrectSave();
	SaveCommand_DuplicateInput_ThrowException();
	DeleteCommand_BadInput_ThrowException();
	DeleteCommand_ValidInput_CorrectRemoval();
	DeleteCommand_ValidInput_CorrectRemovalFromSaved();
	UpdateCommand_BadInput_ThrowException();
	UpdateCommand_ValidInput_CorrectUpdate();
	UpdateCommand_ValidInput_CorrectUpdate_MyList();
	//ListCommand_MassiveValidInput_CorrectExecution();
	ListBySizeAndQuantityOfMicroFragmentsCommand_ValidInput_CorrectExecution();
	ListBySizeCommand_ValidInput_CorrectExecution();
	ListBySizeCommand_ValidInput_DeleteOldIterator();
	ListAll_ValidInput_CorrectExecution();
	ListAll_ValidInput_DeleteOldIterator();
	ListAll_EmptyList_CorrectExecution();
	NextCommand_ValidInput_CorrectExecution();
	NextCommand_ValidInput_CorrectCycle();
	ClearTestFile();
}
