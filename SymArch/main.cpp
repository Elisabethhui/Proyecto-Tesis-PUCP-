#include "mainwindow.h"
// #include "mathlab/bin/generatedCode/first/faceDemoMissingData.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>
#include <iostream>

//#include "engine.h"
// #include "MatlabDataArray.hpp"
// #include "MatlabEngine.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Connect to shared MATLAB session
    //using namespace matlab::engine;
    //std::unique_ptr<MATLABEngine> matlabPtr = connectMATLAB();

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);
    fmt.setVersion(4,5);
    fmt.setProfile(QSurfaceFormat::CoreProfile);

    //std::cout << fmt.version().first << " " << fmt.version().second << std::endl;

    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow mainWindow;
    mainWindow.resize(mainWindow.sizeHint());
    mainWindow.showMaximized();
    return app.exec();
}
