#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QtWidgets>
#include <QDir>
#include <iostream>
#include <fstream>
#include <QKeyEvent>
#include "SimpleMesh/io/offwriter.h"
#include "SimpleMesh/vertex.h"

#ifndef ONLY_INTERFACES
#include "Harris3D/Mesh.h"
#endif

#include "Util/PropertySet.h"

#ifndef ONLY_INTERFACES
#include "rotationaldialog.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/boost/graph/graph_traits_Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/mesh_segmentation.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/property_map.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;
#endif

// using namespace matlab::engine;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_glArea = new GLWidget(this);
    std::cout << "GLWidget created" << endl;

    setCentralWidget(m_glArea);
    createActions();
    createMenus();
    createToolbar();
    createStatusBar();
    createDockWindows();

    std::cout << "Dock created" << endl;

    m_bOpenFile = false;
    setWindowTitle("SymArch");

    m_container = new ObjectContainer;
    m_dock->setObjectContainer(m_container);
    m_dock->setGLWidget(m_glArea);
    m_glArea->setObjectContainer(m_container);

    reader = new SimpleMesh::IO::OFFReader(this);
    wasObjectRead = false;
#ifndef ONLY_INTERFACES
    engineRotational = new Engine::RotationalSymmetry(this);
    engineHeatDiffusion = new Engine::HeatDiffusionSymmetry(this);
#endif


    /*Probablemente estos connect se muevan al constructor*/
    connect(reader, SIGNAL(progress(int, const QString&)), m_progressBar, SLOT(setValue(int, const QString&)));
    connect(reader, SIGNAL(finished()), this, SLOT(finishReadingMesh()));

#ifndef ONLY_INTERFACES
    connect(engineRotational, SIGNAL(progress(int, const QString&)), m_progressBar, SLOT(setValue(int, const QString&)));
    connect(engineRotational, SIGNAL(finished()),this, SLOT(finishRepairRotationalSymmetry()));

    connect(engineHeatDiffusion, SIGNAL(progress(int, const QString&)), m_progressBar, SLOT(setValue(int, const QString&)));
    connect(engineHeatDiffusion, SIGNAL(finished()), this, SLOT(finishRepairHeatDiffusionSymmetry()));
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_container;
    delete m_dock;
}

void MainWindow::createDockWindows(){
    cout << "Mainwindow::createDockWindows" << endl;
    m_dock = new LayerDock;
    addDockWidget(Qt::RightDockWidgetArea, m_dock);

}

void MainWindow::createToolbar(){
    cout << "Mainwindow::createToolbar" << endl;
    ui->mainToolBar->addAction(m_actionOpen);

    m_toolbarMain = addToolBar(tr("Processing"));

#ifndef ONLY_INTERFACES
    m_toolbarMain->addAction(m_actionRotSymmetry);
    m_toolbarMain->addAction(m_actionHeatSymmetry);
#endif

    QWidget* spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_toolbarMain->addWidget(spacer);

    m_toolbarMain->addWidget(m_progressBar);
}

void MainWindow::createActions(){
    cout << "Mainwindow::createActions" << endl;
    m_actionOpen = new QAction(QIcon("images/Open-icon.png"), tr("&Open"), this);
    m_actionOpen->setStatusTip(tr("Open a mesh"));
    connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(open()));

    m_actionAbout = new QAction(tr("About"), this);
    m_actionAbout->setStatusTip(tr("About SymArch"));
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    m_actionExit = new QAction(tr("&Exit"), this);
    m_actionExit->setStatusTip(tr("Exit the application"));
    connect(m_actionExit, SIGNAL(triggered()), this, SLOT(close()));

    m_actionEditBoundingBox = new QAction(tr("&Edit Box"),this);
    m_actionEditBoundingBox->setStatusTip(tr("Edit Bounding Box"));
    connect(m_actionEditBoundingBox, SIGNAL(triggered()), this, SLOT(messageEditBounding()));

    m_actionSelectBestPlanes = new QAction(tr("&Select Planes"),this);
    m_actionSelectBestPlanes->setStatusTip(tr("Select planes"));
    connect(m_actionSelectBestPlanes, SIGNAL(triggered()), this, SLOT(messageSelectPlanes()));

    m_actionRebuild = new QAction(tr("&Rebuild"),this);
    m_actionRebuild->setStatusTip(tr("Rebuild the object"));
    connect(m_actionRebuild, SIGNAL(triggered()), this, SLOT(messageReconstruction()));

    m_actionExport = new QAction(tr("&Export"),this);
    m_actionExport->setStatusTip(tr("Export the object"));
    connect(m_actionExport, SIGNAL(triggered()), this, SLOT(messageExportRebuild()));

#ifndef ONLY_INTERFACES
    m_actionRotSymmetry = new QAction(QIcon("images/administrative-tools-icon.png"),tr("Rotational Symmetry"), this);
    m_actionRotSymmetry->setStatusTip("Repair rotationally symmetric objects");

    m_actionHeatSymmetry = new QAction(QIcon("images/administrative-tools-icon.png"), tr("Heat Diffusion Symmetry"), this);
    m_actionHeatSymmetry->setStatusTip("Repair objects with Heat Diffusion");

    connect(m_actionRotSymmetry, SIGNAL(triggered()), this, SLOT(repairRotationalSymmetry()));
    connect(m_actionHeatSymmetry, SIGNAL(triggered()), this, SLOT(repairHeatDiffusionSymmetry()));
#endif

    m_progressBar = new ProgressBarMessage(this);

}

void MainWindow::createMenus(){
    cout << "Mainwindow::createMenus" << endl;
    m_menuFile = menuBar()->addMenu(tr("&File"));
    m_menuFile->addAction(m_actionOpen);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actionExit);

#ifndef ONLY_INTERFACES
    m_menuProcessing = menuBar()->addMenu(tr("&Processing"));
    m_menuProcessing->addAction(m_actionRotSymmetry);
    m_menuProcessing->addAction(m_actionHeatSymmetry);
#endif

    m_menuHelp = menuBar()->addMenu("&Help");
    m_menuHelp->addAction(m_actionAbout);

    m_editBoundingBox = menuBar()->addMenu(tr("&Edit Bounding Box"));
    m_editBoundingBox->addAction(m_actionEditBoundingBox);
    m_editBoundingBox->setDisabled(true);

    m_menuProcess = menuBar()->addMenu(tr("&Processing"));
    m_menuProcess->addAction(m_actionSelectBestPlanes);
    m_menuProcess->addAction(m_actionRebuild);
    m_menuProcess->addAction(m_actionExport);
    m_menuProcess->setDisabled(true);
    //m_menuProcess->addAction()
}

void MainWindow::createStatusBar(){
    cout << "Mainwindow::createStatusBar" << endl;
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::about(){
    QMessageBox::about(this, tr("About SymArch"),
                       tr("Symmetry Analysis and the application to archaeology"));

}

void MainWindow::messageEditBounding(){
    //GLWidget::valuePressEdit = true;
    cout << "Justo entramos y el valor es " << GLWidget::valuePressEdit << endl;
    int value = QMessageBox::question(this, tr("Edit Bounding Box"),
                       tr("Do yo want to edit the bounding box?"), QMessageBox::Yes|QMessageBox::No);

    if (value == QMessageBox::Yes) {
        GLWidget::valuePressEdit = true;
    } else {
        GLWidget::valuePressEdit = false;
    }
}

void MainWindow::messageSelectPlanes(){
    //GLWidget::valuePressEdit = true;
    //cout << "Justo entramos y el valor es " << GLWidget::valuePressEdit << endl;
    int value = QMessageBox::question(this, tr("Select planes"),
                       tr("Do yo want to select the planes of the object?"), QMessageBox::Yes|QMessageBox::No);

    if (value == QMessageBox::Yes) {
        GLWidget::selectPlanes = true;
        m_glArea->update();
    } else {
        GLWidget::selectPlanes = false;
    }
}

void MainWindow::messageExportRebuild(){
    int value = QMessageBox::question(this, tr("Export the object rebuilded"),
                       tr("Do yo want to export a file .OFF of the object rebuilded?"), QMessageBox::Yes|QMessageBox::No);
    if (value == QMessageBox::Yes) {
        vector<SimpleMesh::Mesh*> listadoObjets = m_container->m_objects;

        ofstream output;
        cout << "Open the File" << endl;
        output.open("rebuild.off");

        //SimpleMesh::Mesh* primerObjeto = listadoObjets[0]; //Primer objeto
        SimpleMesh::Mesh* primerObjeto = m_container->m_objects.at(0);
        SimpleMesh::Mesh* segundoObjeto = objeto; //Segundo objeto

        output << "OFF\n";
        output << primerObjeto->get_number_vertices() + segundoObjeto->get_number_vertices() << " "
               << primerObjeto->get_number_triangles() + segundoObjeto->get_number_triangles() << " 0" << endl;

        //Obtain vertex
        SimpleMesh::Vertex* listadoVertexPrimerObjeto = primerObjeto->get_vertices();
        for(int i = 0; i < primerObjeto->get_number_vertices(); i++){
            output << listadoVertexPrimerObjeto[i].x() << " " << listadoVertexPrimerObjeto[i].y() << " " << listadoVertexPrimerObjeto[i].z() << endl;
        }

        SimpleMesh::Vertex* listadoVertexSegundoObjeto = segundoObjeto->get_vertices();
        for(int i = 0; i < segundoObjeto->get_number_vertices(); i++){
            output << listadoVertexSegundoObjeto[i].x() << " " << listadoVertexSegundoObjeto[i].y() << " " << listadoVertexSegundoObjeto[i].z() << endl;
        }

        //Obtain triangles
        SimpleMesh::Triangle* listadoTrianglesPrimerObjeto = primerObjeto->get_triangles();
        for(int i = 0; i < primerObjeto->get_number_triangles(); i++){
            vector<unsigned int> vert = listadoTrianglesPrimerObjeto[i].get_vertices();
            output << "3 "<< vert[0] << " " << vert[1] << " " << vert[2] << endl;
        }

        SimpleMesh::Triangle* listadoTrianglesSegundoObjeto = segundoObjeto->get_triangles();
        for(int i = 0; i < segundoObjeto->get_number_triangles(); i++){
            vector<unsigned int> vert = listadoTrianglesSegundoObjeto[i].get_vertices();
            output << "3 "<< vert[0] + primerObjeto->get_number_vertices() << " " << vert[1] + primerObjeto->get_number_vertices() << " " << vert[2] + primerObjeto->get_number_vertices() << endl;
        }

        output.close();
        cout << "The file was closed" << endl;
        //Close the file
    }
}

void MainWindow::messageReconstruction(){
    int value = QMessageBox::question(this, tr("Rebuild object"),
                       tr("Do yo want to rebuild the object?"), QMessageBox::Yes|QMessageBox::No);

    if (value == QMessageBox::Yes) {
            GLWidget::rebuild = true;
            //Hacer reconstruccion
            SymArchData::BufferedObject3D* new_buffered_object_rebuild = new SymArchData::BufferedObject3D();
            new_buffered_object_rebuild->setGLWidget(m_glArea);

            SimpleMesh::Plano3D plano = (SimpleMesh::Plano3D)m_container->listadoPlanosX.at(m_container->indiceMejorPlanoX);
            objeto = ((SimpleMesh::Mesh *)m_container->m_objects.at(0))->replicarGeometriaSimetrica(plano,2,-plano.d);

            objeto->get_diagonal();
            objeto->GetSimetriaEstadistica();
            objeto->compute_normals();
            new_buffered_object_rebuild->createBuffersFromObject(objeto);

            //Create the .off file
            ofstream output;
            cout << "Open the File" << endl;
            output.open("text.off");

            output << "OFF\n";
            output << objeto->get_number_vertices() << " " << objeto->get_number_triangles() << " 0" << endl;

            //Obtain vertex
            SimpleMesh::Vertex* listadoVertex = objeto->get_vertices();
            for(int i = 0; i < objeto->get_number_vertices(); i++){
                output << listadoVertex[i].x() << " " << listadoVertex[i].y() << " " << listadoVertex[i].z() << endl;
            }

            //Obtain triangles
            SimpleMesh::Triangle* listadoTriangles = objeto->get_triangles();
            for(int i = 0; i < objeto->get_number_triangles(); i++){
                vector<unsigned int> vert = listadoTriangles[i].get_vertices();
                output << "3 "<< vert[0] << " " << vert[1] << " " << vert[2] << endl;
            }

            output.close();
            cout << "The file was closed" << endl;
            //Close the file

            QString name = "Object rebuild";
            //m_container->addObject(new_mesh, new_buffered_object, nameReadMesh);
            m_container->addObject(objeto,new_buffered_object_rebuild,name);
            int index = 0;
            new_buffered_object_rebuild->setIndex(index);
            new_buffered_object_rebuild->setColor(m_glArea->getColorPalette(5));//Color morado

            //m_container->m_objects.at(0) = objeto;
            cout << "Se reconstruyó"<<endl;
            m_glArea->update();
        } else {
            GLWidget::rebuild = false;
        }
}

void MainWindow::finishReadingMesh(){
    cout << "MainWindow::finishReadingMesh()"<<endl;
    wasObjectRead = true;
    if(wasObjectRead){
        m_editBoundingBox->setEnabled(true);
        m_menuProcess->setEnabled(true);
    }

    //Object
    SimpleMesh::Mesh* new_mesh = reader->get_mesh();
    new_mesh->get_diagonal();
    new_mesh->GetSimetriaEstadistica();
    new_mesh->compute_normals();

    QString textAntes = QTime::currentTime().toString();

    //Aqui colores para el colorBar
    unsigned char* datos_X = new unsigned char[64 * 3 + 1];
    unsigned char* datos_Y = new unsigned char[64 * 3 + 1];
    unsigned char* datos_Z = new unsigned char[64 * 3 + 1];
    int valorX = -1;
    int valorY = -1;
    int valorZ = -1;

    vector<float> datosX;
    vector<float> datosY;
    vector<float> datosZ;


    new_mesh->ComputeBarsSymmetry(datos_X,datos_Y,datos_Z,valorX,valorY,valorZ);

    for(int m = 0; m < 64*3+1;m++){
        m_container->mapcolor_X[m] = datos_X[m];
        m_container->mapcolor_Y[m] = datos_Y[m];
        m_container->mapcolor_Z[m] = datos_Z[m];

        m_container->mapcolor_X_Edit[m] = datos_X[m];
        m_container->mapcolor_Y_Edit[m] = datos_Y[m];
        m_container->mapcolor_Z_Edit[m] = datos_Z[m];
    }

    //Copio los planos al listado de planos que se obtuvieron
    cout << "Los planos X son " << endl;
    for(int i = 0; i < new_mesh->planosX.size(); i++){
        m_container->listadoPlanosX.push_back(new_mesh->planosX[i]);
        cout << "Coord X: " << ((SimpleMesh::Plano3D)new_mesh->planosX[i]).getA() << " ";
        cout << "Coord Y: " << ((SimpleMesh::Plano3D)new_mesh->planosX[i]).getB() << " ";
        cout << "Coord Z: " << ((SimpleMesh::Plano3D)new_mesh->planosX[i]).getC() << " ";
        cout << "Intersecto " << ((SimpleMesh::Plano3D)new_mesh->planosX[i]).getD() << endl;
        m_container->listadoPlanosY.push_back(new_mesh->planosY[i]);
        m_container->listadoPlanosZ.push_back(new_mesh->planosZ[i]);
    }

    //Inicializa los valores de los indices del mejor plano
    m_container->indiceMejorPlanoX = valorX;
    m_container->indiceMejorPlanoY = valorY;
    m_container->indiceMejorPlanoZ = valorZ;

    m_container->indiceActualPlanoX = valorX;
    m_container->indiceActualPlanoY = valorY;
    m_container->indiceActualPlanoZ = valorZ;

    //Asignar valores a los anillos del cilindro
    m_container->mapcolor_X_Edit[valorX*3] = 0;
    m_container->mapcolor_X_Edit[valorX*3 + 1] = 0;
    m_container->mapcolor_X_Edit[valorX*3 + 2] = 0;

    m_container->mapcolor_Y_Edit[valorY*3] = 0;
    m_container->mapcolor_Y_Edit[valorY*3 + 1] = 0;
    m_container->mapcolor_Y_Edit[valorY*3 + 2] = 0;

    m_container->mapcolor_Z_Edit[valorZ*3] = 0;
    m_container->mapcolor_Z_Edit[valorZ*3 + 1] = 0;
    m_container->mapcolor_Z_Edit[valorZ*3 + 2] = 0;

    QString textDespues = QTime::currentTime().toString();

    cout << "Reporte de tiempo:" << endl;
    cout << "Inició a las " << textAntes.toUtf8().constData() << endl;
    cout << "Terminó a las " << textDespues.toUtf8().constData() << endl;
    this->width();

    SymArchData::BufferedObject3D* new_buffered_object = new SymArchData::BufferedObject3D();
    new_buffered_object->setGLWidget(m_glArea);
    new_buffered_object->createBuffersFromObject(new_mesh);

    m_container->addObject(new_mesh, new_buffered_object, nameReadMesh);
    int index = m_container->getNumberObjects() - 1;
    new_buffered_object->setIndex(index);

    new_buffered_object->setColor(m_glArea->getColorPalette(index)); 

    //Color Bar
    float valor1 = m_container->bbox[1][0] - m_container->bbox[0][0];
    float valor2 = m_container->bbox[1][2] - m_container->bbox[0][2];
    float valor3 = m_container->bbox[1][1] - m_container->bbox[0][1];

    float valorMaximo = 1.0;
    if(valor1 >= valor2 && valor1 >= valor3) valorMaximo = valor1;
    else if(valor2 >= valor1 && valor2 >= valor3) valorMaximo = valor2;
    else if(valor3 >= valor1 && valor3 >= valor1) valorMaximo = valor3;

    float radius = valorMaximo*0.04;

    AnalysisBar *analysisBar = new AnalysisBar(10,10,radius);
    float scale = 1.5*m_container->getScale();
    analysisBar->set_scale(scale);

    analysisBar->set_scale(m_container->getScale());
    analysisBar->proceduralInit(new_mesh,m_container->bbox);
    analysisBar->compute_normals();

    SymArchData::BufferedObject3D* new_Color_bar = new SymArchData::BufferedObject3D();
    new_Color_bar->setGLWidget(m_glArea);

    SimpleMesh::Mesh* mesh_analysis_bar = (SimpleMesh::Mesh *)analysisBar;

    new_Color_bar->createBuffersFromObject(mesh_analysis_bar);
    QString nameColorBar = "colorBar";
    m_container->addObject(mesh_analysis_bar, new_Color_bar, nameColorBar);
    int index_1 = m_container->getNumberObjects() - 1;
    new_Color_bar->setIndex(index_1);

    new_Color_bar->setColor(m_glArea->getColorPalette(index_1));

    //Extraer solo el nombre del archivo
    m_dock->addMeshToList(index);
    m_dock->addMeshToList(index_1);

    statusBar()->showMessage(tr("Object loaded"));
    m_progressBar->reset();

    m_bOpenFile = true;
    m_glArea->update();
}

void MainWindow::keyPressEvent(QKeyEvent *e){
    cout << "el boton fue de " << e->key()<<endl;
    setFocus();
}

void MainWindow::open(){
    if(m_bOpenFile){ //Hay modelos abiertos, hay que gestionar la liberación de recursos
        m_container->clearData();
        m_bOpenFile = false;
        m_dock->clearListMeshes();
    }

    QString filename = QFileDialog::getOpenFileName(this, tr("Open mesh"), "./", tr("OFF Files (*.off)"));

    QStringList aux = filename.split("/");
    nameReadMesh = aux.takeLast();
    qDebug() << QDir::separator();

    reader->set_filename(filename.toUtf8().constData());
    reader->read_mesh();
}

#ifndef ONLY_INTERFACES
void MainWindow::repairHeatDiffusionSymmetry(){
    int index = m_container->selection();

    if(index != -1){
        engineHeatDiffusion->setData(m_container->getObject(index), NULL);
        engineHeatDiffusion->start();
    }
}

void MainWindow::finishRepairHeatDiffusionSymmetry(){
    SimpleMesh::Mesh* result = engineHeatDiffusion->getResult();
    result->compute_normals();
    result->get_diagonal();

    SymArchData::BufferedObject3D* new_buffered_object = new SymArchData::BufferedObject3D();
    new_buffered_object->setGLWidget(m_glArea);
    new_buffered_object->createBuffersFromObject(result);

    m_container->addObject(result, new_buffered_object, tr("Result"));
    int index = m_container->getNumberObjects() - 1;
    new_buffered_object->setIndex(index);

    //int ind = m_glArea->addObject(result, new_buffered_object);
    new_buffered_object->setColor(m_glArea->getColorPalette(index));

    m_dock->addMeshToList(index);
    m_progressBar->reset();
    m_glArea->update();
}

/*void MainWindow::saveMesh(){
    int index = m_ListMeshes->currentRow();

    if(index != -1){
        SimpleMesh::IO::OFFWriter writer("aaa.off");
        SimpleMesh::Mesh* m = m_glArea->getMesh(index);
        writer.write_mesh(*m);
    }
}*/

/*void MainWindow::deleteMesh(){
    int index = m_ListMeshes->currentRow();
    if(index!=-1){
       m_glArea->removeMesh(index);
       m_ListMeshes->takeItem(index);
       m_glArea->update();
    }
}*/

/*void MainWindow::mergeMeshes(){
    QModelIndexList indexes = m_ListMeshes->selectionModel()->selectedIndexes();
    std::vector<SimpleMesh::Mesh*> meshes;

    for(int i = 0; i < indexes.size(); i++){
        meshes.push_back(m_glArea->getMesh(indexes[i].row()));
    }

    std::cout << "List retrieved" << std::endl;

    if(meshes.size() > 1){ //Hacer la fusión sólo si hay más de una malla seleccionada
        unsigned int num_total_vertices = 0;
        unsigned int num_total_triangles = 0;

        for(unsigned int i = 0; i < meshes.size(); i++){
            num_total_vertices += meshes[i]->get_number_vertices();
            num_total_triangles += meshes[i]->get_number_triangles();
        }

        SimpleMesh::Mesh* bigMesh = new SimpleMesh::Mesh();
        bigMesh->set_number_vertices(num_total_vertices);
        bigMesh->set_number_triangles(num_total_triangles);

        std::cout << "Merged mesh created" << std::endl;

        unsigned int init_vertices = 0;
        unsigned int init_triangles = 0;

        for(unsigned int i = 0; i < meshes.size(); i++){
            SimpleMesh::Vertex* vertices = meshes[i]->get_vertices();
            SimpleMesh::Triangle* triangles = meshes[i]->get_triangles();

            for(unsigned int j = 0; j < meshes[i]->get_number_vertices(); j++)
                bigMesh->add_vertex(init_vertices + j, vertices[j].x(), vertices[j].y(), vertices[j].z());

            for(unsigned int  j = 0; j < meshes[i]->get_number_triangles(); j++)
                bigMesh->add_triangle(init_triangles + j, init_vertices + triangles[j].get_vertex_at(0), init_vertices + triangles[j].get_vertex_at(1), init_vertices + triangles[j].get_vertex_at(2));

            init_triangles += meshes[i]->get_number_triangles();
            init_vertices += meshes[i]->get_number_vertices();
        }

        std::cout << "Mesh merged" << std::endl;

        bigMesh->compute_normals();
        bigMesh->get_diagonal();

        std::cout << "Normals and diagonal" << std::endl;

        //Remover items de ListWidget y ObjectContainer
        for(int i = indexes.size()-1; i >= 0; i--){
            //std::cout << indexes[i].row() << std::endl;
            m_glArea->removeMesh(indexes[i].row());
            m_ListMeshes->takeItem(indexes[i].row());
        }

        std::cout << "Removed meshes and items" << std::endl;

        SymArchData::BufferedObject3D* new_buffered_object = new SymArchData::BufferedObject3D();
        new_buffered_object->setGLWidget(m_glArea);
        new_buffered_object->createBuffersFromObject(bigMesh);
        int ind = m_glArea->addObject(bigMesh, new_buffered_object);
        new_buffered_object->setColor(m_glArea->getColorPalette(ind));

        std::cout << "To ObjectContainer" << std::endl;

        m_ListMeshes->addItem("Merged");
        m_glArea->update();

    }



}*/

void MainWindow::finishRepairRotationalSymmetry(){
    SimpleMesh::Mesh* result = engineRotational->getResult();
    result->compute_normals();
    result->get_diagonal();

    SymArchData::BufferedObject3D* new_buffered_object = new SymArchData::BufferedObject3D();
    new_buffered_object->setGLWidget(m_glArea);
    new_buffered_object->createBuffersFromObject(result);

    m_container->addObject(result, new_buffered_object, tr("Result"));
    int index = m_container->getNumberObjects() - 1;
    new_buffered_object->setIndex(index);

    //int ind = m_glArea->addObject(result, new_buffered_object);
    new_buffered_object->setColor(m_glArea->getColorPalette(index));

    m_dock->addMeshToList(index);
    m_progressBar->reset();
    m_glArea->update();
}

void MainWindow::repairRotationalSymmetry(){
   //int index = m_ListMeshes->currentRow();
    int index = m_container->selection();

    if(index!=-1){
        //Leer parámetros desde diálogo
        Util::PropertySet* prop = new Util::PropertySet;
        RotationalDialog* dialog = new RotationalDialog(this);
        dialog->setPropertySet(prop);
        int result =  dialog->exec();

        if(result == QDialog::Accepted){
            SimpleMesh::Mesh* originalObject = m_container->getObject(index);
            Harris3D::Mesh* copyObject = new Harris3D::Mesh();
            copyObject->deepCopy(originalObject);
            //cout << "Prop:" << &prop << endl;
            //Engine::RotationalSymmetry engine(copyObject, &prop);
            engineRotational->setData(copyObject, prop);
            // SimpleMesh::Mesh* result = engine.computeCompletion();
            engineRotational->start();
       }


        /*
        prop.addProperty(std::string("type-neighborhood"), std::string("rings"));
        prop.addProperty(std::string("parameter-neighborhood"), std::string("2"));
        prop.addProperty(std::string("K"), std::string("0.04"));
        prop.addProperty(std::string("ring-maxima-detection"), std::string("1"));
        prop.addProperty(std::string("interest-points-selection"), std::string("fraction"));
        prop.addProperty(std::string("parameter-selection"), std::string("0.01"));
        prop.addProperty(std::string("filtering-steps"), std::string("0"));
        prop.addProperty(std::string("alpha"), std::string("0.9"));
        prop.addProperty(std::string("factor-volumetric"), std::string("0.005"));
        prop.addProperty(std::string("num-triangles-decimation"), std::string("80000"));
        prop.addProperty(std::string("iter-non-rigid"), std::string("0"));
        prop.addProperty(std::string("factor-spacing"), std::string("2"));
        prop.addProperty(std::string("compatible-angle"), std::string("0.4"));
        prop.addProperty(std::string("enable-icp"), std::string("0"));
        prop.addProperty(std::string("rotation-factor"), std::string("2"));

        //Obtener la malla seleccionada
        SimpleMesh::Mesh* originalObject = m_glArea->getMesh(index);
        Harris3D::Mesh* copyObject = new Harris3D::Mesh();
        copyObject->deepCopy(originalObject);

        Engine::RotationalSymmetry engine(copyObject, &prop);
        SimpleMesh::Mesh* result = engine.computeCompletion();
        result->compute_normals();
        result->get_diagonal();

        SymArchData::BufferedObject3D* new_buffered_object = new SymArchData::BufferedObject3D();
        new_buffered_object->setGLWidget(m_glArea);
        new_buffered_object->createBuffersFromObject(result);
        int ind = m_glArea->addObject(result, new_buffered_object);
        new_buffered_object->setColor(m_glArea->getColorPalette(ind));

        m_ListMeshes->addItem("Result");
        m_glArea->update();*/

    }
}

/*void MainWindow :: segmentateMesh(){
    int index = m_ListMeshes->currentRow();

    if(index != -1){
        SimpleMesh::Mesh* object = m_glArea->getMesh(index);
        SimpleMesh::Vertex* vertices = object->get_vertices();
        SimpleMesh::Triangle* triangles = object->get_triangles();
        SimpleMesh::IO::OFFWriter writer("temp.off");
        writer.write_mesh(*object);

        Polyhedron p;
        std::ifstream input("temp.off");
        if(!input || !(input >> p) || p.empty()){
            std::cout << "Not a valid off file" << std::endl;
            return;
        }

        //Create a property map for sdf values
        typedef std::map<Polyhedron::Facet_const_handle, double> Facet_double_map;
        Facet_double_map internal_sdf_map;
        boost::associative_property_map<Facet_double_map> sdf_property_map(internal_sdf_map);

        //Compute sdf values
        CGAL::sdf_values(p, sdf_property_map);

        //Create a property map for segment-id
        typedef std::map<Polyhedron::Facet_const_handle, std::size_t> Facet_int_map;
        Facet_int_map internal_segment_map;
        boost::associative_property_map<Facet_int_map> segment_property_map(internal_segment_map);

        //Segmentate the mesh
        std::size_t number_of_segments = CGAL::segmentation_from_sdf_values(p, sdf_property_map, segment_property_map);
        std::cout << "Number of segments:" << number_of_segments << endl;
        std::vector<SimpleMesh::Mesh*> meshes;

        for(unsigned int i = 0; i < number_of_segments; i++){
            //Identificar los vertices de este segmento
            std::vector<bool> mark;
            mark.resize(object->get_number_vertices());

            unsigned int num_faces = 0;
            for(Polyhedron::Facet_iterator facet_it = p.facets_begin(); facet_it!=p.facets_end(); ++facet_it){
                if(segment_property_map[facet_it] == i){
                    Polyhedron::Halfedge_around_facet_circulator circulator = facet_it->facet_begin();
                    int vertex0 = std::distance(p.vertices_begin(), circulator->vertex());  ++circulator;
                    int vertex1 = std::distance(p.vertices_begin(), circulator->vertex());  ++circulator;
                    int vertex2 = std::distance(p.vertices_begin(), circulator->vertex());
                    mark[vertex0] = true;   mark[vertex1] = true; mark[vertex2] = true;
                    num_faces++;
                }
            }

            unsigned int num_vertices = 0;

            std::map<unsigned int, unsigned int> mapping;
            for(unsigned int j = 0; j < mark.size(); j++){
                if(mark[j]){
                    mapping.insert(std::pair<unsigned int, unsigned int>(j, num_vertices));
                    num_vertices++;
                }
            }

            SimpleMesh::Mesh* auxMesh = new SimpleMesh::Mesh();
            auxMesh->set_number_vertices(num_vertices);
            auxMesh->set_number_triangles(num_faces);

            unsigned int cont = 0;
            for(unsigned int j = 0; j < mark.size(); j++){
                if(mark[j]){
                    auxMesh->add_vertex(cont++, vertices[j].x(), vertices[j].y(), vertices[j].z());
                }
            }

            cont = 0;
            for(Polyhedron::Facet_iterator facet_it = p.facets_begin(); facet_it!=p.facets_end(); ++facet_it){
                if(segment_property_map[facet_it] == i){
                    Polyhedron::Halfedge_around_facet_circulator circulator = facet_it->facet_begin();
                    int vertex0 = std::distance(p.vertices_begin(), circulator->vertex());  ++circulator;
                    int vertex1 = std::distance(p.vertices_begin(), circulator->vertex());  ++circulator;
                    int vertex2 = std::distance(p.vertices_begin(), circulator->vertex());
                    auxMesh->add_triangle(cont++, mapping[vertex0], mapping[vertex1], mapping[vertex2]);
                }
            }

            auxMesh->compute_normals();
            auxMesh->get_diagonal();
            if(auxMesh->get_number_vertices() > 100)
                meshes.push_back(auxMesh);
            else
                delete auxMesh;
        }

        for(int i = 0; i < meshes.size(); i++){
            SymArchData::BufferedObject3D* new_buffer = new SymArchData::BufferedObject3D();
            new_buffer->setGLWidget(m_glArea);
            new_buffer->createBuffersFromObject(meshes[i]);
            int ind = m_glArea->addObject(meshes[i], new_buffer);
            new_buffer->setColor(m_glArea->getColorPalette(ind));

            m_ListMeshes->addItem("Segment");
            m_glArea->update();
        }
    }
}*/
#endif



