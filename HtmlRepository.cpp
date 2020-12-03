#include "HtmlRepository.h"
#include <iostream>
#include <fstream>
#include <algorithm>

std::string HtmlRepository::Htmlify(std::vector<OrganicFragment*>* Mylist)
{
	std::string HtmlContent = "";
	HtmlContent += "<!DOCTYPE html>\n";
	HtmlContent += "<html>\n";
	HtmlContent += "\t<head>\n";
	HtmlContent += "\t\t<title>Organic Fragments</title>\n";
	HtmlContent += "\t</head>\n";
	HtmlContent += "\t<body>\n";
	HtmlContent += "\t\t<table border=\"1\">\n";
	HtmlContent += "\t\t<tr>\n";
	HtmlContent += "\t\t\t<td>Id</td>\n";
	HtmlContent += "\t\t\t<td>Size</td>\n";
	HtmlContent += "\t\t\t<td>Infection Level</td>\n";
	HtmlContent += "\t\t\t<td>Quantity of micro fragments</td>\n";
	HtmlContent += "\t\t\t<td>Photograph</td>\n";
	HtmlContent += "\t\t</tr>\n";
	for (auto Element : *Mylist)
	{
		HtmlContent += "\t\t<tr>\n";
		HtmlContent += "\t\t\t<td>" + Element->GetId() + "</td>\n";
		HtmlContent += "\t\t\t<td>" + Element->GetSize() + "</td>\n";
		HtmlContent += "\t\t\t<td>" + std::to_string(Element->GetInfectionLevel()) + "</td>\n";
		HtmlContent += "\t\t\t<td>" + std::to_string(Element->GetQuantityOfMicroFragments()) + "</td>\n";
		HtmlContent += "\t\t\t<td>" + Element->GetPhotograph() + "</td>\n";
		HtmlContent += "\t\t</tr>\n";
	}
	HtmlContent += "\t\t</table>\n";
	HtmlContent += "\t</body>\n";
	HtmlContent += "</html>\n";
	return HtmlContent;
}

HtmlRepository::HtmlRepository(std::string Path) : Path(Path) {}

void HtmlRepository::AddFragment(OrganicFragment* SomeOrganicFragment)
{
	auto TemporaryStorage = Filter();
	std::ofstream File(Path);
	TemporaryStorage->push_back(SomeOrganicFragment);
	File << Htmlify(TemporaryStorage);
	File.close();
	for (auto Element : *TemporaryStorage)
		delete Element;
	delete TemporaryStorage;
}

void HtmlRepository::DeleteFragment(std::string OrganicFragmentId)
{
	auto TemporaryStorage = Filter();
	std::ofstream File(Path);
	std::function<bool(OrganicFragment*)> FilterFunction = [&OrganicFragmentId](OrganicFragment* Element)
	{
		return Element->GetId() == OrganicFragmentId;
	};
	auto NewEnd = remove_if(TemporaryStorage->begin(), TemporaryStorage->end(), FilterFunction);
	auto NewEndCopy = NewEnd;
	while (NewEndCopy != TemporaryStorage->end())
	{
		delete* NewEndCopy;
		NewEndCopy++;
	}
	TemporaryStorage->erase(NewEnd, TemporaryStorage->end());
	File << Htmlify(TemporaryStorage);
	File.close();
	for (auto Element : *TemporaryStorage)
		delete Element;
	delete TemporaryStorage;
}

void HtmlRepository::UpdateFragment(std::string OrganicFragmentId, OrganicFragment* NewOrganicFragment)
{
	auto TemporaryStorage = Filter();
	std::ofstream File(Path);
	std::function<bool(OrganicFragment*)> FilterFunction = [&OrganicFragmentId](OrganicFragment* Element)
	{
		return Element->GetId() == OrganicFragmentId;
	};
	replace_if(TemporaryStorage->begin(), TemporaryStorage->end(), FilterFunction, NewOrganicFragment);
	File << Htmlify(TemporaryStorage);
	File.close();
}

OrganicFragment* HtmlRepository::Search(std::string OrganicFragmentId)
{
	auto TemporaryStorage = Filter();
	OrganicFragment* ReturnValue = NULL;
	for (auto Element : *TemporaryStorage)
		if (Element->GetId() == OrganicFragmentId)
		{
			ReturnValue = Element;
		}
		else delete Element;
	delete TemporaryStorage;
	return ReturnValue;
}

std::vector<OrganicFragment*>* HtmlRepository::Filter(std::function<bool(OrganicFragment*)> FilterFunction)
{
	std::ifstream File(Path);
	std::vector<OrganicFragment*>* TemporaryStorage = new std::vector<OrganicFragment*>;
	OrganicFragment* StoredElement;
	std::string Line;
	int CountTillActualContent = 5;
	while (getline(File, Line))
	{
		if (Line.substr(0, 7) == "\t\t\t<td>")
		{
			if (CountTillActualContent != 0)
				CountTillActualContent--;
			else
			{
				int StartPosition = Line.find('>') + 1;
				std::string Id = Line.substr(StartPosition, Line.rfind('<') - StartPosition);
				getline(File, Line);
				std::string Size = Line.substr(StartPosition, Line.rfind('<') - StartPosition);
				getline(File, Line);
				std::string InfectionLevel = Line.substr(StartPosition, Line.rfind('<') - StartPosition);
				getline(File, Line);
				std::string QuantityOfMicroFragments = Line.substr(StartPosition, Line.rfind('<') - StartPosition);
				getline(File, Line);
				std::string Photograph = Line.substr(StartPosition, Line.rfind('<') - StartPosition);
				StoredElement = new OrganicFragment(Id, Size, std::stof(InfectionLevel), std::stoi(QuantityOfMicroFragments), Photograph);
				if (FilterFunction(StoredElement) == true)
					TemporaryStorage->push_back(StoredElement);
				else delete StoredElement;
			}
		}
	}
	File.close();
	return TemporaryStorage;
}

void HtmlRepository::Open()
{
	std::string Command = "start chrome \"" + Path + "\""; // \"C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe\"
	system(Command.c_str());
}
