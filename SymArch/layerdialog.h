#ifndef LAYERDIALOG_H
#define LAYERDIALOG_H

#include <QDockWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class BufferedObject3D;

class LayerDialog : public QDockWidget
{
    Q_OBJECT
public:
    LayerDialog(QWidget* parent = 0);
    ~LayerDialog();

public Q_SLOTS:
    void listItemClicked(QTreeWidgetItem*, int);

private:
    QTreeWidget* m_listWidget;
    void addTreeWidgetItem(QTreeWidgetItem* parent, BufferedObject3D* mm);
};

#endif // LAYERDIALOG_H
