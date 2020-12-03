#include "Domain.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <vector>
#include <sstream>
// Create an organic fragment
OrganicFragment::OrganicFragment(std::string NewOrganicFragmentId, std::string NewOrganicFragmentSize, float NewOrganicFragmentInfectionLevel , int NewOrganicFragmentQuantityOfMicroFragments, std::string NewOrganicFragmentPhotograph)
{
	Id = std::string(NewOrganicFragmentId);
	SetSize(NewOrganicFragmentSize);
	SetInfectionLevel(NewOrganicFragmentInfectionLevel);
	SetQuantityOfMicroFragments(NewOrganicFragmentQuantityOfMicroFragments);
	SetPhotograph(NewOrganicFragmentPhotograph);
}
// Constructor to create a copy from an existing organic fragment
OrganicFragment::OrganicFragment(OrganicFragment* SomeOrganicFragment)
{
	Id = std::string(SomeOrganicFragment->Id);
	SetSize(SomeOrganicFragment->Size);
	SetInfectionLevel(SomeOrganicFragment->InfectionLevel);
	SetQuantityOfMicroFragments(SomeOrganicFragment->QuantityOfMicroFragments);
	SetPhotograph(SomeOrganicFragment->Photograph);
}
// Empty Constructor
OrganicFragment::OrganicFragment()
{
	Id = "";
	SetSize("");
	SetInfectionLevel(0);
	SetQuantityOfMicroFragments(0);
	SetPhotograph("");
}
// Getters
std::string OrganicFragment::GetId()
{
	return std::string(Id);
}
std::string OrganicFragment::GetSize()
{
	return std::string(Size);
}
float OrganicFragment::GetInfectionLevel()
{
	return InfectionLevel;
}
int OrganicFragment::GetQuantityOfMicroFragments()
{
	return QuantityOfMicroFragments;
}
std::string OrganicFragment::GetPhotograph()
{
	return std::string(Photograph);
}
// Setters
void OrganicFragment::SetSize(std::string NewSize)
{
	Size = std::string(NewSize);
}
void OrganicFragment::SetInfectionLevel(float NewInfectionLevel)
{
	InfectionLevel = NewInfectionLevel;
}
void OrganicFragment::SetQuantityOfMicroFragments(int NewQuantityOfMicroFragments)
{
	QuantityOfMicroFragments = NewQuantityOfMicroFragments;
}
void OrganicFragment::SetPhotograph(std::string NewPhotograph)
{
	Photograph = std::string(NewPhotograph);
}

std::ostream& operator<<(std::ostream& OutputStream, OrganicFragment* Element)
{
	OutputStream << Element->GetId() << "," << Element->GetSize() << "," << Element->GetInfectionLevel() << "," << Element->GetQuantityOfMicroFragments() << ',' << Element->GetPhotograph() << "\n";
	return OutputStream;
}

std::istream& operator>>(std::istream& InputStream, OrganicFragment* Element)
{
	std::string Line;
	std::string Property;
	getline(InputStream, Line);
	std::stringstream LineStream(Line);
	std::vector<std::string> ElementProperties;
	while (getline(LineStream, Property, ','))
		ElementProperties.push_back(Property);
	if (ElementProperties.size() == 0)
		return InputStream;
	
	std::string OrganicFragmentId = ElementProperties[0];
	std::string OrganicFragmentSize = ElementProperties[1];
	std::string OrganicFragmentInfectionLevel = ElementProperties[2];
	std::string OrganicFragmentQuantityOfMicroFragments = ElementProperties[3];
	std::string OrganicFragmentPhotograph = ElementProperties[4];
	Element->Id = OrganicFragmentId;
	Element->SetSize(OrganicFragmentSize);
	Element->SetInfectionLevel(std::stof(OrganicFragmentInfectionLevel));
	Element->SetQuantityOfMicroFragments(std::stoi(OrganicFragmentQuantityOfMicroFragments));
	Element->SetPhotograph(OrganicFragmentPhotograph);

	return InputStream;
}
