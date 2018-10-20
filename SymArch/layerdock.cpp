#include <QtWidgets>
#include <iostream>
#include "layerdock.h"


LayerDock::LayerDock(QWidget* parent) : QDockWidget(parent)
{
    setFeatures(QDockWidget::NoDockWidgetFeatures);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_ListMeshes = new QTreeWidget(this);
    m_ListMeshes->setSelectionMode(QTreeWidget::ExtendedSelection);

    m_ListMeshes->clear();
    m_ListMeshes->setColumnCount(2);
    m_ListMeshes->setColumnWidth(0,32);
    m_ListMeshes->setColumnWidth(1,40);
    m_ListMeshes->setIndentation(10);
    m_ListMeshes->setSizeIncrement(0,1);

    QSizePolicy qsp(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
    qsp.setHorizontalStretch(0);
    qsp.setVerticalStretch(2);

    m_ListMeshes->setSizePolicy(qsp);
    m_ListMeshes->header()->hide();

    QWidget* dockContainer = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_ListMeshes);

    QPushButton* pushDeleteMesh = new QPushButton(QIcon("images/trash.png"), tr(""), this);
    QPushButton* pushMergeMeshes = new QPushButton(QIcon("images/merge.png"), tr(""), this);
    QPushButton* pushSegmentateMesh = new QPushButton(QIcon("images/segmentate.png"), tr(""), this);
    QPushButton* pushSaveMesh = new QPushButton(QIcon("images/save.png"), tr(""), this);

    connect(pushDeleteMesh, SIGNAL(pressed()), this, SLOT(deleteMesh()));

    connect(m_ListMeshes, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(meshItemClicked(QTreeWidgetItem *, int)));

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(pushDeleteMesh);
    buttonLayout->addWidget(pushMergeMeshes);
    buttonLayout->addWidget(pushSegmentateMesh);
    buttonLayout->addWidget(pushSaveMesh);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    dockContainer->setLayout(mainLayout);
    setWidget(dockContainer);
}

LayerDock::~LayerDock(){

}

void LayerDock::addMeshToList(int index){
    MeshTreeWidgetItem *item = new MeshTreeWidgetItem(m_container, index);
    m_ListMeshes->addTopLevelItem(item);
}

void LayerDock::meshItemClicked(QTreeWidgetItem* item, int col){
    MeshTreeWidgetItem* mItem = dynamic_cast<MeshTreeWidgetItem *> (item);
    if(mItem){
        int clickedId = mItem->id();

        if(col == 0){
            m_container->setVisibility(clickedId, !m_container->getVisibility(clickedId));
        }

        m_container->select(clickedId);
        std::cout << "Selected item(" << clickedId << ")" << std::endl;
        updateWidget();
        m_glArea->update();
    }
}

MeshTreeWidgetItem::MeshTreeWidgetItem(ObjectContainer *m, int ind){
    index = ind;
    if(m->getVisibility(index))
        setIcon(0, QIcon("images/layer_eye_open.png"));
    else
        setIcon(0, QIcon("images/layer_eye_close.png"));

    setText(1, m->getName(index));

}

void LayerDock::updateWidget(){
    m_ListMeshes->clear();
    m_ListMeshes->setColumnCount(2);
    m_ListMeshes->setColumnWidth(0,32);
    m_ListMeshes->setColumnWidth(1,40);
    m_ListMeshes->setIndentation(10);
    m_ListMeshes->setSizeIncrement(0,1);

    QSizePolicy qsp(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
    qsp.setHorizontalStretch(0);
    qsp.setVerticalStretch(2);

    m_ListMeshes->setSizePolicy(qsp);
    m_ListMeshes->header()->hide();

    //Re-insert items for each object in the container
    for(int i = 0; i < m_container->getNumberObjects(); i++){
        MeshTreeWidgetItem* item = new MeshTreeWidgetItem(m_container, i);
        if(m_container->selection() == i){
            item->setBackground(1, QBrush(Qt::yellow));
            item->setForeground(1, QBrush(Qt::blue));
        }

        m_ListMeshes->addTopLevelItem(item);
    }

}

void LayerDock::deleteMesh(){
    std::cout << m_container->selection() << std::endl;
    if(m_container->selection()!=-1){
        m_container->removeObject(m_container->selection());
        m_container->select(-1);
        updateWidget();
        m_glArea->update();
    }
}
