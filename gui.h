#pragma once
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include "ui_Lab11.h"
#include "Service.h"
#include <string>
#include <vector>
#include "Observer.h"
//
class GUI
{
public:
	static int Start(int& argc, char** argv);
	static Service* Controller;
	static Repository* Mylist;
	static Repository* Storage;
	static QWidget* OpenWindow;
	static bool InMemory;
	static std::vector<Observer*> Observers;
	static void RegisterObserver(Observer*);
	static void UnregisterObserver(Observer*);
	static void NotifyAll();
};

class Lab11 : public QMainWindow
{
	Q_OBJECT
public:
	Lab11(QWidget* = Q_NULLPTR);
private:
	std::string PathToFile;
	std::string PathToMylist;
	Ui::Lab11Class GUI;
	void BuildWindow();
	QMenu* FileMenu;
	void FileLocation();
	void MylistLocation();
	void OpenAdminMode();
	void OpenUserMode();
	void OpenListOrganicFragmentMenu();
	void BuildListWindow(QWidget*, std::vector<OrganicFragment*>*);
	void BuildMylistWindow(QWidget*, std::vector<OrganicFragment*>*);
	void DrawGraph();
	void OpenAddOrganicFragmentMenu();
	void OpenDeleteOrganicFragmentMenu();
	void OpenUpdateOrganicFragmentMenu();
	void OpenSaveOrganicFragmentMenu();
	void OpenFilterOrganicFragmentMenu();
};