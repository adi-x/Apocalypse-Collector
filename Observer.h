#pragma once
class Observer
{
public:
	virtual void Update() = 0;
	virtual ~Observer() = 0 {};
};