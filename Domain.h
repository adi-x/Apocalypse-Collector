#pragma once
#include <string>
#include <iostream>
class OrganicFragment 
{
private:
	std::string Id;
	std::string Size;
	float InfectionLevel;
	int QuantityOfMicroFragments;
	std::string Photograph;
public:
	// Create an organic fragment
	OrganicFragment(std::string NewOrganicFragmentId, std::string NewOrganicFragmentSize, float NewOrganicFragmentInfectionLevel, int NewOrganicFragmentQuantityOfMicroFragments, std::string NewOrganicFragmentPhotograph);

	// Constructor to create a copy from an existing organic fragment
	OrganicFragment(OrganicFragment* SomeOrganicFragment);

	// Empty Constructor
	OrganicFragment();

	// Getters
	std::string GetId();
	std::string GetSize();
	float GetInfectionLevel();
	int GetQuantityOfMicroFragments();
	std::string GetPhotograph();

	// Setters
	void SetSize(std::string NewOrganicFragmentSize);
	void SetInfectionLevel(float NewOrganicFragmentInfectionLevel);
	void SetQuantityOfMicroFragments(int NewOrganicFragmentQuantityOfMicroFragments);
	void SetPhotograph(std::string NewOrganicFragmentPhotograph);

	//
	friend std::ostream& operator<<(std::ostream&, OrganicFragment*);
	friend std::istream& operator>>(std::istream&, OrganicFragment*);
};