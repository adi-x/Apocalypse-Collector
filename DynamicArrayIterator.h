#include <vector>
#include "Domain.h"
class IteratorPair {
private:
	std::vector<OrganicFragment*>::iterator ActualPositionIterator;
	std::vector<OrganicFragment*>* List;
public:
	// Constructor (the iterating list is cached)
	IteratorPair(std::vector<OrganicFragment*>* List)
	{
		this->List = List;
		ActualPositionIterator = this->List->begin();
	}

	// Go to the next element
	void Next()
	{
		if (ActualPositionIterator == List->end())// this happens when it's empty...
			return;
		ActualPositionIterator++;
		if (ActualPositionIterator == List->end())
			ActualPositionIterator = List->begin();
	}

	// returns current element 
	OrganicFragment* GetCurrent()
	{
		if (ActualPositionIterator == List->end())
			return NULL;
		return new OrganicFragment(*ActualPositionIterator);
	}

	// Destructor (removes the cached list as well)
	~IteratorPair()
	{
		for (auto Element : *List)
			delete Element;
		delete List;
	}

};
