#include <iostream>
#include "gui.h"
#include "Tests.h"


int main(int argc, char *argv[])
{
	int OperationSuccesStatus = GUI::Start(argc, argv);
	return OperationSuccesStatus;
}
