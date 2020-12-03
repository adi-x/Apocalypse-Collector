#include "mylist_view.h"
#include <QFont>
#include <QBrush>
#include <QTableView>
#include <QLayout>
#include "Observer.h"
#include <QPainter>
MylistModel::MylistModel(Repository& Storage, QObject* Parent) : QAbstractTableModel(Parent), Storage(Storage) { }

MylistModel::~MylistModel() { }

inline void ClearVector(std::vector<OrganicFragment*>* Data)
{
	for (auto Element : *Data)
		delete Element;
	delete Data;
}

int MylistModel::rowCount(const QModelIndex& parent) const
{
	auto Data = Storage.Filter();
	int Count = Data->size();
	ClearVector(Data);
	return Count;
}

int MylistModel::columnCount(const QModelIndex& parent) const
{
	return 5;
}

QVariant MylistModel::data(const QModelIndex& index, int role) const
{
	int Row = index.row();
	int Column = index.column();
	auto Data = Storage.Filter();
	if (Row == Data->size())
	{
		ClearVector(Data);
		return QVariant();
	}

	OrganicFragment* Element = (*Data)[Row];
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		QString Information;
		switch (Column)
		{
		case 0:
			Information = QString::fromStdString(Element->GetId());
			ClearVector(Data);
			return Information;
		case 1:
			Information = QString::fromStdString(Element->GetSize());
			ClearVector(Data);
			return Information;
		case 2:
			Information = QString::fromStdString(std::to_string(Element->GetInfectionLevel()));
			ClearVector(Data);
			return Information;
		case 3:
			Information = QString::fromStdString(std::to_string(Element->GetQuantityOfMicroFragments()));
			ClearVector(Data);
			return Information;
		case 4:
			Information = QString::fromStdString(Element->GetPhotograph());
			ClearVector(Data);
			return Information;
		default: break;
		}
	}
	ClearVector(Data);
	if (role == Qt::FontRole)
	{
		//QFont font("Times", 15, 10, true);
		//font.setItalic(false);
		return QVariant();
	}
	if (role == Qt::BackgroundRole)
	{
		if (Row % 2 == 1)
		{
			//return QBrush{ QColor{0, 250, 154} };
		}
	}

	return QVariant{};

}

QVariant MylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			switch (section)
			{
			case 0:
				return QString{ "Id" };
			case 1:
				return QString{ "Size" };
			case 2:
				return QString{ "Infection level" };
			case 3:
				return QString{ "Quantity of micro fragments" };
			case 4:
				return QString{ "Photograph" };
			default:
				break;
			}
		}
	}
	if (role == Qt::FontRole)
	{
		return QVariant();
	}

	return QVariant{};
}

bool MylistModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}

Qt::ItemFlags MylistModel::flags(const QModelIndex& index) const
{
	return Qt::ItemFlags();
}

void MylistModel::reset()
{
	this->beginResetModel();
	this->endResetModel();
}

void MylistWindow::Setup()
{
	TableView = new QTableView;
	TableView->setModel(TableModel);
	TableView->setItemDelegate(new PictureDelegate);
	TableView->resizeColumnsToContents();
	this->setLayout(new QVBoxLayout());
	this->layout()->addWidget(TableView);
	this->setFixedSize(438, 200);
	this->show();
}

MylistWindow::MylistWindow(Repository& Storage, QWidget* Parent) : Storage(Storage), QWidget(Parent)
{
	TableModel = new MylistModel(Storage);
	Setup();
}
MylistWindow::~MylistWindow()
{
	
}

void MylistWindow::Update()
{
	TableModel->reset();
}

void MylistWindow::closeEvent(QCloseEvent* Event)
{
	this->setObjectName(this->objectName() + "_closed");
	QWidget::closeEvent(Event);
}

PictureDelegate::PictureDelegate(QWidget* parent) : QStyledItemDelegate(parent) { }

void PictureDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QString Photograph = index.model()->data(index, Qt::EditRole).toString();

	if (index.column() != 4) // Photograph column only
	{
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}

	QPixmap PixelMapDrawing(Photograph);
	painter->drawPixmap(option.rect, PixelMapDrawing);
}

QSize PictureDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// show a picture only in the last column
	if (index.column() == 4)
	{
		return QSize(32, 100);
	}

	return QStyledItemDelegate::sizeHint(option, index);
}
