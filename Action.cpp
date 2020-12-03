#include "Action.h"

AddAction::AddAction(OrganicFragment* Element, Repository& Storage) : Element(new OrganicFragment(Element)), Storage(Storage) { }

AddAction::~AddAction()
{
	delete Element;
}

void AddAction::Redo()
{
	Storage.AddFragment(new OrganicFragment(Element));
}

void AddAction::Undo()
{
	Storage.DeleteFragment(Element->GetId());
}

RemoveAction::RemoveAction(OrganicFragment* Element, Repository& Storage) : Element(new OrganicFragment(Element)), Storage(Storage) { }

RemoveAction::~RemoveAction()
{
	delete Element;
}

void RemoveAction::Redo()
{
	Storage.DeleteFragment(Element->GetId());
}

void RemoveAction::Undo()
{
	Storage.AddFragment(new OrganicFragment(Element));
}

UpdateAction::UpdateAction(OrganicFragment* OldElement, OrganicFragment* NewElement, Repository& Storage) : OldElement(new OrganicFragment(OldElement)), NewElement(new OrganicFragment(NewElement)), Storage(Storage) { }

UpdateAction::~UpdateAction()
{
	delete OldElement; delete NewElement;
}

void UpdateAction::Redo()
{
	Storage.UpdateFragment(NewElement->GetId(), new OrganicFragment(NewElement));
}

void UpdateAction::Undo()
{
	Storage.UpdateFragment(OldElement->GetId(), new OrganicFragment(OldElement));
}
