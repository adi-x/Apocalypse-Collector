#pragma once
#include "AbstractRepository.h"
#include "Domain.h"
class Action
{
public:
	virtual ~Action() = 0 {};
	virtual void Redo() = 0;
	virtual void Undo() = 0;
};

class AddAction : public Action
{
private:
	Repository& Storage;
	OrganicFragment* Element;
public:
	AddAction(OrganicFragment*, Repository&);
	~AddAction() override;
	void Redo() override;
	void Undo() override;
};

class RemoveAction : public Action
{
private:
	Repository& Storage;
	OrganicFragment* Element;
public:
	RemoveAction(OrganicFragment*, Repository&);
	~RemoveAction() override;
	void Redo() override;
	void Undo() override;
};

class UpdateAction : public Action
{
private:
	Repository& Storage;
	OrganicFragment* OldElement;
	OrganicFragment* NewElement;
public:
	UpdateAction(OrganicFragment*, OrganicFragment*, Repository&);
	~UpdateAction() override;
	void Redo() override;
	void Undo() override;
};