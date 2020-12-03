#pragma once
#include <QAbstractTableModel>
#include "AbstractRepository.h"
#include <QWidget>
#include <QWindow>
#include <QTableView>
#include "Observer.h"
#include <QStyledItemDelegate>
class MylistModel : public QAbstractTableModel
{
private:
	Repository& Storage;
public:
	MylistModel(Repository& Storage, QObject* Parent = NULL);
	~MylistModel();
	int rowCount(const QModelIndex& parent = QModelIndex{}) const override;
	int columnCount(const QModelIndex& parent = QModelIndex{}) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	void reset();
};

class PictureDelegate : public QStyledItemDelegate
{
public:
	PictureDelegate(QWidget* parent = 0);
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};



class MylistWindow : public QWidget, public Observer
{
	Q_OBJECT
private:
	Repository& Storage;
	
	void Setup();
public:
	MylistModel* TableModel;
	QTableView* TableView;
	MylistWindow(Repository&, QWidget* = NULL);
	~MylistWindow() override;
	void Update() override;
	void closeEvent(QCloseEvent*);
};