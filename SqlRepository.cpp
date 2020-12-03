#include "SqlRepository.h"
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
static void CleanVector(std::vector<OrganicFragment*>& SomeList)
{
	std::for_each(SomeList.begin(), SomeList.end(),
		[&SomeList](OrganicFragment* Element)
		{
			delete Element;
		});
}

// Constructor (creates the file if it doesn't exist)
OrganicFragmentSqlRepository::OrganicFragmentSqlRepository(std::string FileLocation)
{
	this->FileLocation = FileLocation;
	sqlite3 *Database;
	int OperationSucces;
	OperationSucces = sqlite3_open(FileLocation.c_str(), &Database);
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
	sqlite3_close(Database);
}

// Destructor
OrganicFragmentSqlRepository::~OrganicFragmentSqlRepository() {}

// Takes the contents of the file stored in the repo and creates a vector out of it
std::vector<OrganicFragment*> OrganicFragmentSqlRepository::LoadData()
{
	sqlite3* Database;
	int OperationSucces;
	std::vector<OrganicFragment*> TemporaryStorage;
	OrganicFragment* StoredElement;
	//
	OperationSucces = sqlite3_open(FileLocation.c_str(), &Database);
	if (OperationSucces != 0)
		throw AppException("Could not open database!");
	//
	std::string SqlToGetElements = "SELECT * FROM OrganicFragments";
	char* SqlErrorMessage;
	//
	auto Func = [](void* VoidTemporaryStorage, int ColumnCount, char** Row, char** Column)  {
		const int IdIndex = 0, SizeIndex = 1, InfectionLevelIndex = 2, QuantityOfMicroFragmentsIndex = 3, PhotographIndex = 4;
		std::vector<OrganicFragment*>* TemporaryStorage = (std::vector<OrganicFragment*>*) VoidTemporaryStorage;
		OrganicFragment* StoredElement = new OrganicFragment(std::string(Row[IdIndex]), std::string(Row[SizeIndex]), std::stof(std::string(Row[InfectionLevelIndex])), std::stoi(std::string(Row[QuantityOfMicroFragmentsIndex])), std::string(Row[PhotographIndex]));
		TemporaryStorage->push_back(StoredElement);
		return 0;
	};
	OperationSucces = sqlite3_exec(Database, SqlToGetElements.c_str(), Func, &TemporaryStorage, &SqlErrorMessage);
	//
	sqlite3_close(Database);
	if (OperationSucces != SQLITE_OK)
		throw AppException((std::string("Could not retrieve data!") + std::string(SqlErrorMessage)).c_str());
	//
	return TemporaryStorage;
}

// Function to add a new element in the repo (appends it to the end of the list)
void OrganicFragmentSqlRepository::AddFragment(OrganicFragment* SomeOrganicFragment)
{
	sqlite3* Database;
	int OperationSucces;
	OperationSucces = sqlite3_open(FileLocation.c_str(), &Database);
	if (OperationSucces != 0)
	{
		delete SomeOrganicFragment;
		throw AppException("Could not open database!");
	}
	std::stringstream SqlToAddElementStream;
	SqlToAddElementStream << "INSERT INTO OrganicFragments (Id, Size, InfectionLevel, QuantityOfMicroFragments, Photograph) VALUES('";
	SqlToAddElementStream << SomeOrganicFragment->GetId() << "', '" << SomeOrganicFragment->GetSize() << "', " << SomeOrganicFragment->GetInfectionLevel();
	SqlToAddElementStream << ", " << SomeOrganicFragment->GetQuantityOfMicroFragments() << ", '" << SomeOrganicFragment->GetPhotograph() << "');";
	std::string SqlToAddElement = SqlToAddElementStream.str();
	char* SqlErrorMessage;
	OperationSucces = sqlite3_exec(Database, SqlToAddElement.c_str(), NULL, 0, &SqlErrorMessage);
	sqlite3_close(Database);
	delete SomeOrganicFragment;
	if (OperationSucces != SQLITE_OK)
		throw AppException((std::string("Could not add to database! ") + std::string(SqlErrorMessage)).c_str());
}

// Function to delete an element from the repo
void OrganicFragmentSqlRepository::DeleteFragment(std::string OrganicFragmentId)
{
	sqlite3* Database;
	int OperationSucces;
	OperationSucces = sqlite3_open(FileLocation.c_str(), &Database);
	if (OperationSucces != 0)
		throw AppException("Could not open database!");
	std::stringstream SqlToDeleteElementStream;
	SqlToDeleteElementStream << "DELETE FROM OrganicFragments WHERE Id='" << OrganicFragmentId << "';";
	std::string SqlToDeleteElement = SqlToDeleteElementStream.str();
	char* SqlErrorMessage;
	OperationSucces = sqlite3_exec(Database, SqlToDeleteElement.c_str(), NULL, 0, &SqlErrorMessage);
	sqlite3_close(Database);
	if (OperationSucces != SQLITE_OK)
		throw AppException((std::string("Could not delete from database! ") + std::string(SqlErrorMessage)).c_str());
	delete SqlErrorMessage;
}

// Function to update an element from the repo
void OrganicFragmentSqlRepository::UpdateFragment(std::string OrganicFragmentId, OrganicFragment* NewOrganicFragment)
{
	sqlite3* Database;
	int OperationSucces;
	OperationSucces = sqlite3_open(FileLocation.c_str(), &Database);
	if (OperationSucces != 0)
	{
		delete NewOrganicFragment;
		throw AppException("Could not open database!");
	}
	std::stringstream SqlToUpdateElementStream;
	SqlToUpdateElementStream << "UPDATE OrganicFragments SET Size='";//(Id, Size, InfectionLevel, QuantityOfMicroFragments, Photograph) VALUES('";
	SqlToUpdateElementStream << NewOrganicFragment->GetSize() << "', InfectionLevel=" << NewOrganicFragment->GetInfectionLevel();
	SqlToUpdateElementStream << ", QuantityOfMicroFragments=" << NewOrganicFragment->GetQuantityOfMicroFragments() << ", Photograph='"; 
	SqlToUpdateElementStream << NewOrganicFragment->GetPhotograph() << "' WHERE Id = '" << NewOrganicFragment->GetId() << "';";
	std::string SqlToUpdateElement = SqlToUpdateElementStream.str();
	char* SqlErrorMessage;
	OperationSucces = sqlite3_exec(Database, SqlToUpdateElement.c_str(), NULL, 0, &SqlErrorMessage);
	sqlite3_close(Database);
	delete NewOrganicFragment;
	if (OperationSucces != SQLITE_OK)
		throw AppException((std::string("Could not update database! ") + std::string(SqlErrorMessage)).c_str());
}

// Function to search for an element in the repo
OrganicFragment* OrganicFragmentSqlRepository::Search(std::string OrganicFragmentId)
{
	sqlite3* Database;
	int OperationSucces;
	OrganicFragment* ReturnValue = NULL;
	//
	OperationSucces = sqlite3_open(FileLocation.c_str(), &Database);
	if (OperationSucces != 0)
		throw AppException("Could not open database!");
	//
	std::stringstream SqlToFindElementStream;
	SqlToFindElementStream << "SELECT * FROM OrganicFragments WHERE Id='" << OrganicFragmentId <<"';";
	std::string SqlToFindElement = SqlToFindElementStream.str();
	char* SqlErrorMessage;
	//
	auto Func = [](void* ReturnValue, int ColumnCount, char** Row, char** Column) {
		const int IdIndex = 0, SizeIndex = 1, InfectionLevelIndex = 2, QuantityOfMicroFragmentsIndex = 3, PhotographIndex = 4;
		OrganicFragment** StoredElement = (OrganicFragment**) ReturnValue;
		*StoredElement = new OrganicFragment(std::string(Row[IdIndex]), std::string(Row[SizeIndex]), std::stof(std::string(Row[InfectionLevelIndex])), std::stoi(std::string(Row[QuantityOfMicroFragmentsIndex])), std::string(Row[PhotographIndex]));
		return 0;
	};
	OperationSucces = sqlite3_exec(Database, SqlToFindElement.c_str(), Func, &ReturnValue, &SqlErrorMessage);
	//
	sqlite3_close(Database);
	if (OperationSucces != SQLITE_OK)
		throw AppException((std::string("Could not search for data! ") + std::string(SqlErrorMessage)).c_str());
	//
	return ReturnValue;
}

// Function to filter the list from repo
std::vector<OrganicFragment*>* OrganicFragmentSqlRepository::Filter(std::function<bool(OrganicFragment*)> FilterFunction)
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