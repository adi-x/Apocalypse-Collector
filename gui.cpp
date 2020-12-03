#include "gui.h"
#include <QStyle>
#include <QDesktopWidget>
#include <QPushButton>
#include <QtWidgets>
#include <QVBoxLayout>
#include "FileRepository.h"
#include "SqlRepository.h"
#include "AbstractRepository.h"
#include <QTableView>
#include "qcustomplot.h"
#include "ui_AdminMode.h"
#include "ui_Add.h"
#include "ui_Delete.h"
#include "AppException.h"
#include "ui_UserMode.h"
#include <fstream>
#include "ui_Filter.h"
#include "mylist_view.h"
#include <QTableView>
#include <QAbstractItemModel>
#include <algorithm>
#include "HtmlRepository.h"
#include "CsvRepository.h"

Service* GUI::Controller = NULL;
QWidget* GUI::OpenWindow = NULL;
Repository* GUI::Mylist = NULL;
Repository* GUI::Storage = NULL;
std::vector<Observer*> GUI::Observers;
bool GUI::InMemory = false;

Lab11::Lab11(QWidget* Parent) : QMainWindow(Parent)
{
	PathToFile = "";
	PathToMylist = "";
	GUI.setupUi(this);
	GUI::OpenWindow = this;
	BuildWindow();
}

int GUI::Start(int& argc, char** argv)
{
	QApplication App(argc, argv);
	//App.setQuitOnLastWindowClosed(false);
	Lab11 Window;
	Window.show();
	
	return App.exec();
}

void GUI::RegisterObserver(Observer* SomeObserver)
{
	Observers.push_back(SomeObserver);
}

void GUI::UnregisterObserver(Observer* SomeObserver)
{
	auto NewEnd = std::remove(Observers.begin(), Observers.end(), SomeObserver);
	Observers.erase(NewEnd, Observers.end());
}

void GUI::NotifyAll()
{
	for (auto ObserverObject : Observers)
		ObserverObject->Update();
}

void Lab11::OpenAdminMode()
{
	Ui::AdminMode* AdminForm = new Ui::AdminMode;
	auto AdminWidget = new QWidget();
	AdminForm->setupUi(AdminWidget);
	AdminWidget->show();
	AdminWidget->setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			AdminWidget->size(),
			qApp->desktop()->availableGeometry()
		)
	);
	auto UndoLambda = [=]() {
		try
		{
			GUI::Controller->UndoStorage();
		}
		catch (AppException& error)
		{
			QMessageBox msg;
			msg.setText(QString::fromStdString(error.what()));
			msg.exec();
		}
	};
	auto RedoLambda = [=]() {
		try
		{
			GUI::Controller->RedoStorage();
		}
		catch (AppException& error)
		{
			QMessageBox msg;
			msg.setText(QString::fromStdString(error.what()));
			msg.exec();
		}
	};
	QObject::connect(AdminForm->AddButton, &QPushButton::clicked, this, &Lab11::OpenAddOrganicFragmentMenu);
	QObject::connect(AdminForm->DeleteButton, &QPushButton::clicked, this, &Lab11::OpenDeleteOrganicFragmentMenu);
	QObject::connect(AdminForm->UpdateButton, &QPushButton::clicked, this, &Lab11::OpenUpdateOrganicFragmentMenu);
	QObject::connect(AdminForm->ListButton, &QPushButton::clicked, this, &Lab11::OpenListOrganicFragmentMenu);
	QObject::connect(AdminForm->GraphButton, &QPushButton::clicked, this, &Lab11::DrawGraph);
	QObject::connect(AdminForm->UndoButton, &QPushButton::clicked, UndoLambda);
	QObject::connect(AdminForm->RedoButton, &QPushButton::clicked, RedoLambda);
	auto CtrlZ = new QShortcut(QKeySequence("Ctrl+Z"), AdminWidget);
	QObject::connect(CtrlZ, &QShortcut::activated, UndoLambda);
	auto CtrlY = new QShortcut(QKeySequence("Ctrl+Y"), AdminWidget);
	QObject::connect(CtrlY, &QShortcut::activated, RedoLambda);
	
}

void SetWidgetToFragment(QListWidget* List, OrganicFragment* Element)
{
	List->clear();
	List->addItem("Id: " + QString::fromStdString(Element->GetId()));
	List->addItem("Size: " + QString::fromStdString(Element->GetSize()));
	List->addItem("Infection Level: " + QString::fromStdString(std::to_string(Element->GetInfectionLevel())));
	List->addItem("Quantity Of Micro Fragments: " + QString::fromStdString(std::to_string(Element->GetQuantityOfMicroFragments())));
	List->addItem("Photograph: " + QString::fromStdString(Element->GetPhotograph()));
}

void Lab11::OpenUserMode()
{
	Ui::UserMode* UserForm = new Ui::UserMode;
	auto UserWidget = new QWidget();
	UserForm->setupUi(UserWidget);
	UserWidget->show();
	UserWidget->setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			UserWidget->size(),
			qApp->desktop()->availableGeometry()
		)
	);
	auto ListView = UserForm->ItemDisplay;
	//auto Fragment = GUI::Controller->NextCommand();
	auto Fragment = GUI::Controller->ListAllCommand();
	if (Fragment != NULL)
		SetWidgetToFragment(ListView, Fragment);
	if (GUI::InMemory == true)
		UserForm->OpenButton->setDisabled(true);
	QObject::connect(UserForm->NextButton, &QPushButton::clicked, this, [=]() {
		auto Fragment = GUI::Controller->NextCommand();
		if (Fragment != NULL)
			SetWidgetToFragment(ListView, Fragment);
	});
	auto UndoLambda = [=]() {
		try
		{
			GUI::Controller->UndoMylist();
			GUI::NotifyAll();
		}
		catch (AppException& error)
		{
			QMessageBox msg;
			msg.setText(QString::fromStdString(error.what()));
			msg.exec();
		}
	};
	auto RedoLambda = [=]() {
		try
		{
			GUI::Controller->RedoMylist();
			GUI::NotifyAll();
		}
		catch (AppException& error)
		{
			QMessageBox msg;
			msg.setText(QString::fromStdString(error.what()));
			msg.exec();
		}
	};
	QObject::connect(UserForm->SaveButton, &QPushButton::clicked, this, &Lab11::OpenSaveOrganicFragmentMenu);
	QObject::connect(UserForm->FilterButton, &QPushButton::clicked, this, &Lab11::OpenFilterOrganicFragmentMenu);
	QObject::connect(UserForm->OpenButton, &QPushButton::clicked, this, []() {
		GUI::Controller->OpenMylistCommand();
	});
	QObject::connect(UserForm->DisplayButton, &QPushButton::clicked, this, [=]() {
		//auto Window = new QWidget;
		//BuildListWindow(Window, GUI::Controller->MyListCommand());
		//Window->show();
		BuildMylistWindow(NULL, NULL);
	});
	QObject::connect(UserForm->UndoButton, &QPushButton::clicked, UndoLambda);
	QObject::connect(UserForm->RedoButton, &QPushButton::clicked, RedoLambda);
}

void Lab11::OpenListOrganicFragmentMenu()
{
	auto Window = new QWidget();
	//GUI::OpenWindow->hide();
	BuildListWindow(Window, GUI::Controller->ListCommand());
	Window->show();
}

void Lab11::BuildListWindow(QWidget* Window, std::vector<OrganicFragment*>* DisplayList)
{
	GUI::OpenWindow = Window;
	Window->setFixedSize(QSize(500, 200));
	Window->setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			Window->size(),
			qApp->desktop()->availableGeometry()
		)
	);
	
	//auto DisplayList = GUI::Controller->ListCommand();
	auto List = new QTableWidget();
	QStringList ListHeader;
	ListHeader.append("Id");
	ListHeader.append("Size");
	ListHeader.append("Infection Level");
	ListHeader.append("Quantity of Micro Fragments");
	ListHeader.append("Photograph");

	List->setColumnCount(5);
	List->setHorizontalHeaderLabels(ListHeader);
	List->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	List->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	
	int Row = 0;
	for (auto Element : *DisplayList)
	{
		List->insertRow(Row);
		auto Item = new QTableWidgetItem();
		Item->setText(QString::fromStdString(Element->GetId()));
		List->setItem(Row, 0, Item);
		Item = new QTableWidgetItem();
		Item->setText(QString::fromStdString(Element->GetSize()));
		List->setItem(Row, 1, Item);
		Item = new QTableWidgetItem();
		Item->setText(QString::fromStdString(std::to_string(Element->GetInfectionLevel())));
		List->setItem(Row, 2, Item);
		Item = new QTableWidgetItem();
		Item->setText(QString::fromStdString(std::to_string(Element->GetQuantityOfMicroFragments())));
		List->setItem(Row, 3, Item);
		Item = new QTableWidgetItem();
		Item->setText(QString::fromStdString(Element->GetPhotograph()));
		List->setItem(Row, 4, Item);
		Row++;
	}

	auto MainLayout = new QVBoxLayout;
	MainLayout->addWidget(List);
	Window->setLayout(MainLayout);
}

void Lab11::BuildMylistWindow(QWidget* Window, std::vector<OrganicFragment*>* DisplayList)
{
	auto Widget = new MylistWindow(*GUI::Mylist);
	GUI::RegisterObserver((Observer*) Widget);
	QObject::connect(Widget, &QWidget::objectNameChanged, [=]() { // fast way of getting close event
		GUI::UnregisterObserver((Observer*) Widget);
		});
}

void Lab11::DrawGraph()
{
	auto Window = new QWidget();
	//Window->setFixedSize(QSize(1000, 600));
	Window->setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			Window->size(),
			qApp->desktop()->availableGeometry()
		)
	);
	// Create empty bar chart objects:
	auto Plot = new QCustomPlot();
	auto Layout = new QVBoxLayout();
	Window->setLayout(Layout);
	Layout->addWidget(Plot);
	auto Fragments = GUI::Controller->ListCommand();

	QCPBars* InfectionLevelBar = new QCPBars(Plot->xAxis, Plot->yAxis);
	QCPBars* QuantityOfMicroFragmentsBar = new QCPBars(Plot->xAxis, Plot->yAxis);
	auto BarGroup = new QCPBarsGroup(Plot);
	InfectionLevelBar->setWidth(0.4);
	QuantityOfMicroFragmentsBar->setWidth(0.4);
	BarGroup->append(InfectionLevelBar);
	BarGroup->append(QuantityOfMicroFragmentsBar);
	BarGroup->setSpacing(0);

	// Gives more crisp, pixel aligned bar borders
	InfectionLevelBar->setAntialiased(false);
	QuantityOfMicroFragmentsBar->setAntialiased(false);
	
	// Set names and colors:
	InfectionLevelBar->setName("Infection level");
	InfectionLevelBar->setPen(QPen(QColor(255, 0, 0).lighter()));
	InfectionLevelBar->setBrush(QColor(255, 0, 0));
	QuantityOfMicroFragmentsBar->setName("Quantity of Micro Fragments");
	QuantityOfMicroFragmentsBar->setPen(QPen(QColor(204, 204, 255).lighter()));
	QuantityOfMicroFragmentsBar->setBrush(QColor(204, 204, 255));

	// prepare x axis with country labels:
	QVector<double> Ticks;
	QVector<QString> Labels;
	int ActualTick = 1;
	QVector<double> InfectionLevelData, QuantityOfMicroFragmentsData;
	for (auto Fragment : *Fragments)
	{
		Ticks << ActualTick;
		Labels << QString::fromStdString(Fragment->GetId());
		InfectionLevelData << Fragment->GetInfectionLevel();
		QuantityOfMicroFragmentsData << Fragment->GetQuantityOfMicroFragments();
		ActualTick++;
	}
	//ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;
	//labels << "USA" << "Japan" << "Germany" << "France" << "UK" << "Italy" << "Canada";
	QSharedPointer<QCPAxisTickerText> TextTicker(new QCPAxisTickerText);
	TextTicker->addTicks(Ticks, Labels);
	Plot->xAxis->setTicker(TextTicker);
	Plot->xAxis->setTickLabelRotation(60);
	Plot->xAxis->setSubTicks(false);
	Plot->xAxis->setTickLength(0, 2);
	Plot->xAxis->setRange(0, Fragments->size() + 1);
	Plot->xAxis->setBasePen(QPen(Qt::black));
	Plot->xAxis->setTickPen(QPen(Qt::black));
	Plot->xAxis->grid()->setVisible(true);
	Plot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
	Plot->xAxis->setTickLabelColor(Qt::black);
	Plot->xAxis->setLabelColor(Qt::black);

	// prepare y axis:
	//Plot->yAxis->setRange(0, 12.1);
	Plot->yAxis->setRangeUpper(20);
	Plot->yAxis->setPadding(5); // a bit more space to the left border
	Plot->yAxis->setLabel("Infection Level and Quantity of Micro Fragments per Organic Fragment");
	Plot->yAxis->setBasePen(QPen(Qt::black));
	Plot->yAxis->setTickPen(QPen(Qt::black));
	Plot->yAxis->setSubTickPen(QPen(Qt::black));
	Plot->yAxis->grid()->setSubGridVisible(true);
	Plot->yAxis->setTickLabelColor(Qt::black);
	Plot->yAxis->setLabelColor(Qt::black);
	Plot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
	Plot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

	// Add data:
	InfectionLevelBar->setData(Ticks, InfectionLevelData);
	QuantityOfMicroFragmentsBar->setData(Ticks, QuantityOfMicroFragmentsData);

	// setup legend:
	Plot->legend->setVisible(true);
	Plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignHCenter);
	Plot->legend->setBrush(QColor(255, 255, 255, 100));
	Plot->legend->setBorderPen(Qt::NoPen);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	Plot->legend->setFont(legendFont);

	Window->show();
}

void Lab11::OpenAddOrganicFragmentMenu()
{
	Ui::AddFormW* AddFormWidget = new Ui::AddFormW;
	auto Widget = new QWidget();
	AddFormWidget->setupUi(Widget);
	Widget->show();
	QObject::connect(AddFormWidget->CancelButton, &QPushButton::clicked, Widget, &QWidget::close);
	QObject::connect(AddFormWidget->ConfirmButton, &QPushButton::clicked, [=]() {
		//qDebug() << AddFormWidget->IdLine->text();
		if(AddFormWidget->IdLine->text() == "" || AddFormWidget->SizeLine->text() == "" || AddFormWidget->PhotographLine->text() == "")
		{
			AddFormWidget->Error->setText("You cannot leave a field empty!");
			return;
		}
		GUI::Controller->AddCommand(
			AddFormWidget->IdLine->text().toStdString(),
			AddFormWidget->SizeLine->text().toStdString(),
			AddFormWidget->InfectionLevelLine->text().toFloat(),
			AddFormWidget->QuantityOfMicroFragmentsLine->text().toInt(),
			AddFormWidget->PhotographLine->text().toStdString()
		);
		Widget->close();
	});
}

void Lab11::OpenDeleteOrganicFragmentMenu()
{
	Ui::DeleteFormW* DeleteFormWidget = new Ui::DeleteFormW;
	auto Widget = new QWidget();
	DeleteFormWidget->setupUi(Widget);
	Widget->show();
	QObject::connect(DeleteFormWidget->CancelButton, &QPushButton::clicked, Widget, &QWidget::close);
	QObject::connect(DeleteFormWidget->ConfirmButton, &QPushButton::clicked, [=]() {
		//qDebug() << DeleteFormWidget->IdLine->text();
		if (DeleteFormWidget->IdLine_2->text() == "")
		{
			DeleteFormWidget->Error->setText("You cannot leave a field empty!");
			return;
		}
		try {
			GUI::Controller->DeleteCommand(DeleteFormWidget->IdLine_2->text().toStdString());
			// If it got here it was removed
			Widget->close();
		}
		catch (AppException &Error)
		{
			DeleteFormWidget->Error->setText(Error.what());
		}
	});
}

void Lab11::OpenUpdateOrganicFragmentMenu()
{
	Ui::AddFormW* UpdateFormWidget = new Ui::AddFormW; // We'll reuse AddForm
	auto Widget = new QWidget();
	UpdateFormWidget->setupUi(Widget);
	Widget->show();
	Widget->setWindowTitle("Update");
	QObject::connect(UpdateFormWidget->CancelButton, &QPushButton::clicked, Widget, &QWidget::close);
	QObject::connect(UpdateFormWidget->ConfirmButton, &QPushButton::clicked, [=]() {
		//qDebug() << AddFormWidget->IdLine->text();
		if (UpdateFormWidget->IdLine->text() == "" || UpdateFormWidget->SizeLine->text() == "" || UpdateFormWidget->PhotographLine->text() == "")
		{
			UpdateFormWidget->Error->setText("You cannot leave a field empty!");
			return;
		}
		try {
			GUI::Controller->UpdateCommand(
				UpdateFormWidget->IdLine->text().toStdString(),
				UpdateFormWidget->SizeLine->text().toStdString(),
				UpdateFormWidget->InfectionLevelLine->text().toFloat(),
				UpdateFormWidget->QuantityOfMicroFragmentsLine->text().toInt(),
				UpdateFormWidget->PhotographLine->text().toStdString()
			);
			// If it got here it was updated
			Widget->close();
		}
		catch (AppException& Error)
		{
			UpdateFormWidget->Error->setText(Error.what());
		}
		
	});
}

void Lab11::OpenSaveOrganicFragmentMenu()
{
	Ui::DeleteFormW* SaveFormWidget = new Ui::DeleteFormW;
	auto Widget = new QWidget();
	//DeleteFormWidget->IdLabel_2->setText("Id to save");
	
	SaveFormWidget->setupUi(Widget);
	Widget->setWindowTitle("Save");
	SaveFormWidget->IdLabel_2->setText("Id to save");
	Widget->show();
	QObject::connect(SaveFormWidget->CancelButton, &QPushButton::clicked, Widget, &QWidget::close);
	QObject::connect(SaveFormWidget->ConfirmButton, &QPushButton::clicked, [=]() {
		//qDebug() << DeleteFormWidget->IdLine->text();
		if (SaveFormWidget->IdLine_2->text() == "")
		{
			SaveFormWidget->Error->setText("You cannot leave a field empty!");
			return;
		}
		try {
			GUI::Controller->Save(SaveFormWidget->IdLine_2->text().toStdString());
			// If it got here it was removed
			GUI::NotifyAll();
			Widget->close();
		}
		catch (AppException& Error)
		{
			SaveFormWidget->Error->setText(Error.what());
		}
		});
}

void Lab11::OpenFilterOrganicFragmentMenu()
{
	Ui::FilterForm* FilterFormWidget = new Ui::FilterForm;
	auto Widget = new QWidget();
	FilterFormWidget->setupUi(Widget);
	Widget->show();
	QObject::connect(FilterFormWidget->CancelButton, &QPushButton::clicked, Widget, &QWidget::close);
	QObject::connect(FilterFormWidget->ConfirmButton, &QPushButton::clicked, [=]() {
		//qDebug() << AddFormWidget->IdLine->text();
		if (FilterFormWidget->SizeLine->text() == "")
		{
			FilterFormWidget->Error->setText("You cannot leave a field empty!");
			return;
		}
		auto FilteredList = GUI::Controller->ListBySizeAndQuantityOfMicroFragmentsCommand(
			FilterFormWidget->SizeLine->text().toStdString(),
			FilterFormWidget->QuantityOfMicroFragmentsLine->text().toInt()
		);
		auto Window = new QWidget();
		//GUI::OpenWindow->hide();
		BuildListWindow(Window, FilteredList);
		Window->show();
		Widget->close();
		});
}

void Lab11::FileLocation()
{
	QString PathToFile = QFileDialog::getOpenFileName(this, "Select a file", "/", "Storage Files (*.db *.txt *.html *.csv)");
	if (PathToFile != "")
	{
		if (GUI::Controller != NULL)
			delete GUI::Controller;
		this->PathToFile = PathToFile.toStdString();
		if (this->PathToFile != "")
		{
			if (this->PathToFile.back() == 't')
				GUI::Storage = new OrganicFragmentFileRepository(this->PathToFile);
			else if (this->PathToFile.back() == 'l')
				GUI::Storage = new HtmlRepository(this->PathToFile);
			else if (this->PathToFile.back() == 'v')
				GUI::Storage = new CsvRepository(this->PathToFile);
			else if (this->PathToFile.back() == 'b')
				GUI::Storage = new OrganicFragmentSqlRepository(this->PathToFile);
			this->FileMenu->actions()[0]->setEnabled(false);
		}
		if (GUI::Storage != NULL && GUI::Mylist != NULL)
		{
			GUI::Controller = new Service(GUI::Storage, GUI::Mylist);
			this->centralWidget()->layout()->itemAt(0)->widget()->setDisabled(false);
			this->centralWidget()->layout()->itemAt(1)->widget()->setDisabled(false);
		}
	}
}

void Lab11::MylistLocation()
{
	QString PathToMylist = QFileDialog::getOpenFileName(this, "Select a file", "/", "Storage Files (*.db *.txt *.html *.csv)");
	if (PathToMylist != "")
	{
		if (GUI::Controller != NULL)
			delete GUI::Controller;
		this->PathToMylist = PathToMylist.toStdString();
		if (this->PathToMylist != "")
		{
			if (this->PathToMylist.back() == 't')
				GUI::Mylist = new OrganicFragmentFileRepository(this->PathToMylist);
			else if (this->PathToMylist.back() == 'l')
				GUI::Mylist = new HtmlRepository(this->PathToMylist);
			else if (this->PathToMylist.back() == 'v')
				GUI::Mylist = new CsvRepository(this->PathToMylist);
			else if (this->PathToMylist.back() == 'b')
				GUI::Mylist = new OrganicFragmentSqlRepository(this->PathToMylist);
			this->FileMenu->actions()[1]->setEnabled(false);
		}
		if (GUI::Storage != NULL && GUI::Mylist != NULL)
		{
			GUI::Controller = new Service(GUI::Storage, GUI::Mylist);
			this->centralWidget()->layout()->itemAt(0)->widget()->setDisabled(false);
			this->centralWidget()->layout()->itemAt(1)->widget()->setDisabled(false);
		}
	}
}

void Lab11::BuildWindow()
{
	// Creating a menu
	auto Window = new QWidget{this};
	this->FileMenu = this->menuBar()->addMenu("&File");

	// Creating actions
	auto OpenFileAction = new QAction("Open Storage File");
	auto OpenMylistAction = new QAction("Open Mylist File");
	auto ExitAction = new QAction("Exit");
	this->FileMenu->addAction(OpenFileAction);
	this->FileMenu->addAction(OpenMylistAction);
	this->FileMenu->addSeparator();
	this->FileMenu->addAction(ExitAction);
	this->setFixedSize(QSize(200, 300));
	OpenFileAction->setObjectName("StorageAction");
	//OpenMylistAction->setEnabled(false);
	this->setGeometry(
		QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			this->size(),
			qApp->desktop()->availableGeometry()
		)
	);

	// Creating the basic buttons
	auto Layout = new QVBoxLayout(this);
	auto AdminMode = new QPushButton("Administrator Mode");
	auto UserMode = new QPushButton("User Mode");

	Layout->addWidget(AdminMode);
	Layout->addWidget(UserMode);
	
	UserMode->setDisabled(true);
	AdminMode->setDisabled(true);
	AdminMode->setMaximumHeight(150);
	UserMode->setMaximumHeight(150);
	AdminMode->setObjectName("AdminModeButton");

	Window->setLayout(Layout);
	this->show();
	this->setCentralWidget(Window);

	// Adding functionality to everything so far
	QObject::connect(ExitAction, &QAction::triggered, qApp, &QApplication::exit);
	QObject::connect(OpenFileAction, &QAction::triggered, this, &Lab11::FileLocation);
	QObject::connect(OpenMylistAction, &QAction::triggered, this, &Lab11::MylistLocation);
	QObject::connect(AdminMode, &QPushButton::clicked, this, &Lab11::OpenAdminMode);
	QObject::connect(UserMode, &QPushButton::clicked, this, &Lab11::OpenUserMode);

	std::ifstream f(".config");
	std::string Line;
	getline(f, Line);
	if (Line.substr(0, 8) == "inmemory")
	{
		Line.erase(0, Line.find('=') + 1);
		if (Line[0] == ' ')
			Line.erase(0, 1);
		if (Line.back() == ' ')
			Line.pop_back();
		if (Line == "true")
		{
			GUI::InMemory = true;
			GUI::Storage = new InmemoryRepository;
			GUI::Mylist = new InmemoryRepository;
			GUI::Controller = new Service(GUI::Storage, GUI::Mylist);
			//GUI::Controller->AddCommand("1", "1", 1, 1, "1");
			//MylistWindow* _= new MylistWindow(*Repo);
			//GUI::Controller->AddCommand("2", "1", 1, 1, "1");
			//_->TableView->setModel(_->TableModel);
			//_->TableModel->reset();
			//_->TableView->repaint();
			this->centralWidget()->layout()->itemAt(0)->widget()->setDisabled(false);
			this->centralWidget()->layout()->itemAt(1)->widget()->setDisabled(false);
			OpenFileAction->setDisabled(true);
			OpenMylistAction->setDisabled(true);
		}
	}
	f.close();
}
