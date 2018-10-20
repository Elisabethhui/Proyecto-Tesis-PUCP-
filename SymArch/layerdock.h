#ifndef LAYERDOCK_H
#define LAYERDOCK_H

#include <QDockWidget>
#include <QTreeWidget>
#include "objectcontainer.h"
#include "glwidget.h"

//class ObjectContainer;

class MeshTreeWidgetItem : public QTreeWidgetItem
{
public:
    MeshTreeWidgetItem(ObjectContainer*, int );
    int id(){return index;}
    int index;
};

class LayerDock : public QDockWidget
{
    Q_OBJECT

public:
    LayerDock(QWidget* parent=0);
    ~LayerDock();

    void clearListMeshes(){m_ListMeshes->clear();}
    void setObjectContainer(ObjectContainer* oc){m_container = oc;}
    void setGLWidget(GLWidget* wid){ m_glArea = wid;}
    void addMeshToList(int index);
    void updateWidget();

public Q_SLOTS:
    void meshItemClicked(QTreeWidgetItem* item, int col);
    void deleteMesh();

private:
    QTreeWidget* m_ListMeshes;
    ObjectContainer* m_container;
    GLWidget* m_glArea;
};

#endif // LAYERDOCK_H
