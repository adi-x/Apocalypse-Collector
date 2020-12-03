#pragma once
#include "Domain.h"
#include <functional>
#include <vector>
#include "AbstractRepository.h"
class OrganicFragmentFileRepository : public Repository
{
private:
	std::string FileLocation;

	std::vector<OrganicFragment*> LoadData();
	void OverwriteData(std::vector<OrganicFragment*> TemporaryStorage);
public:
	// Constructor
	OrganicFragmentFileRepository(std::string FileLocation);

	// Destructor
	~OrganicFragmentFileRepository();

	// Setters
	void AddFragment(OrganicFragment* SomeOrganicFragment) override;
	void DeleteFragment(std::string OrganicFragmentId) override;
	void UpdateFragment(std::string OrganicFragmentId, OrganicFragment* NewOrganicFragment) override;

	// Getters
	OrganicFragment* Search(std::string OrganicFragmentId) override;
	std::vector<OrganicFragment*>* Filter(std::function<bool(OrganicFragment*)> = [](OrganicFragment*) {return true; }) override; // Initial declaration using a lambda function
	void Open() override;
};