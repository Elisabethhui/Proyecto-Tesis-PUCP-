#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "glwidget.h"
#include "objectcontainer.h"
#include "layerdock.h"
#include "progressbarmessage.h"
#include "SimpleMesh/io/offreader.h"

#ifndef ONLY_INTERFACES
#include "RotationalSymmetry/rotationalsymmetry.h"
#include "HeatDiffusionSymmetry/heatdiffusionsymmetry.h"
#endif

class BufferedObject3D;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *e);
    ~MainWindow();

private Q_SLOTS:
    void about();
    void messageEditBounding();
    void open();
    void finishReadingMesh();
    void messageSelectPlanes();
    void messageReconstruction();
    void messageExportRebuild();
#ifndef ONLY_INTERFACES
    void repairRotationalSymmetry();
    void finishRepairRotationalSymmetry();

    void repairHeatDiffusionSymmetry();
    void finishRepairHeatDiffusionSymmetry();
#endif

private:

    void createActions();
    void createMenus();
    void createToolbar();
    void createStatusBar();
    void createDockWindows();



    Ui::MainWindow *ui;

    //GUI controls
    QToolBar*   m_toolbarMain;
    QMenu*      m_menuFile;
    QMenu*      m_menuProcessing;
    QMenu*      m_menuHelp;
    QMenu*      m_editBoundingBox;
    QMenu*      m_menuProcess; //Selected planes

    QAction*    m_actionOpen;
    QAction*    m_actionExit;
    QAction*    m_actionEditBoundingBox;
    QAction*    m_actionSelectBestPlanes;
    QAction*    m_actionRebuild;
    QAction*    m_actionExport;
public:
    bool wasObjectRead = false;
    static QString valorTeclado;

    SimpleMesh::Mesh* objeto;
    //QString getValorTeclado();
#ifndef ONLY_INTERFACES
    QAction*    m_actionRotSymmetry;
    QAction*    m_actionHeatSymmetry;
#endif

    QAction*    m_actionAbout;

    ProgressBarMessage* m_progressBar;

    bool m_bOpenFile;
    GLWidget*   m_glArea;
    LayerDock*  m_dock;
    ObjectContainer* m_container;

    SimpleMesh::Mesh* new_mesh;
    SimpleMesh::Mesh* mesh_rebuild;
    QString nameReadMesh;

    SimpleMesh::IO::OFFReader* reader;

#ifndef ONLY_INTERFACES
    Engine::RotationalSymmetry* engineRotational;
    Engine::HeatDiffusionSymmetry* engineHeatDiffusion;
#endif
};

#endif // MAINWINDOW_H
