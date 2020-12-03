#pragma once
#include <string>
#include <vector>
#include "Domain.h"
#include "AbstractRepository.h"
class CsvRepository : public Repository
{
private:
	std::string Path;
public:
	// Constructor
	CsvRepository(std::string Path);

	// Setters
	void AddFragment(OrganicFragment* SomeOrganicFragment) override;
	void DeleteFragment(std::string OrganicFragmentId) override;
	void UpdateFragment(std::string OrganicFragmentId, OrganicFragment* NewOrganicFragment) override;

	// Getters
	virtual OrganicFragment* Search(std::string OrganicFragmentId) override;
	virtual std::vector<OrganicFragment*>* Filter(std::function<bool(OrganicFragment*)> = [](OrganicFragment*) {return true; }) override;
	void Open() override;
};