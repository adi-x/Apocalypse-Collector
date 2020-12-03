#pragma once
#include <string>
#include <vector>
#include "Domain.h"
#include "AbstractRepository.h"
class HtmlRepository : public Repository
{
private:
	std::string Path;
	std::string Htmlify(std::vector<OrganicFragment*>*);
public:
	// Constructor
	HtmlRepository(std::string Path);

	// Setters
	void AddFragment(OrganicFragment* SomeOrganicFragment) override;
	void DeleteFragment(std::string OrganicFragmentId) override;
	void UpdateFragment(std::string OrganicFragmentId, OrganicFragment* NewOrganicFragment) override;

	// Getters
	OrganicFragment* Search(std::string OrganicFragmentId) override;
	std::vector<OrganicFragment*>* Filter(std::function<bool(OrganicFragment*)> = [](OrganicFragment*) {return true; }) override; // Initial declaration using a lambda function
	void Open() override;
};