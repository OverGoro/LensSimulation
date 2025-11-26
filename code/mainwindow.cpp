#include "mainwindow.h"
#include <qgridlayout.h>
#include "polygonalmodel.h"
#include "sphere.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);

    _renderingWidget = new RenderingWidget(this);
    _renderingWidget->setMinimumWidth(400);
    _renderingWidget->setMinimumHeight(400);
    _sceneWidget = new SceneWidget(this);

    QGridLayout *layout = new QGridLayout(centralWidget);
    layout->addWidget(_renderingWidget, 0, 0, 1, 1);
    layout->addWidget(_sceneWidget,     0, 1, 1, 1);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    _drawManager = std::make_shared<DrawManager>(std::make_shared<Drawer>(_renderingWidget, this), this);
    _sceneManager = std::make_shared<SceneManager>(_sceneWidget, this);

    _sceneWidget->loadDrawer(_drawManager->drawer());


    connect(_sceneWidget, &SceneWidget::renderSignal, this, &MainWindow::renderFrame);
    connect(_sceneWidget, &SceneWidget::updateMapSignal, this, &MainWindow::updateMap);

    Polygon p (Vec3(-3, 3.5, 3.5), Vec3(-3, -3.5, 3.5), Vec3(-3, -3.5, -3.5), Vec3(1,1,1), 0, 1, 0);
    Polygon p1(Vec3(-3, 3.5, 3.5), Vec3(-3, 3.5, -3.5), Vec3(-3, -3.5, -3.5), Vec3(1,1,1), 0, 1, 0);
    Polygon p2(Vec3(3.5, 3.5, 3), Vec3(-3.5, 3.5, 3), Vec3(-3.5, -3.5, 3), Vec3(1,1,1), 0, 1, 0);
    Polygon p3(Vec3(3.5, 3.5, 3), Vec3(3.5, -3.5, 3), Vec3(-3.5, -3.5, 3), Vec3(1,1,1), 0, 1, 0);


    Polygon pm1(Vec3(-1, 2, 2), Vec3(-1, 0, 2), Vec3(-1, 2, 0), Vec3(1,1,0.4), 1, 1.5, 0);
    Polygon pm2(Vec3(-2, 2, 2), Vec3(-2, 0, 2), Vec3(-2, 2, 0), Vec3(1,1,0.4), 1, 1.5, 0);
    Polygon pm3(Vec3(-1, 2, 2), Vec3(-2, 2, 2), Vec3(-2, 2, 0), Vec3(1,1,0.4), 1, 1.5, 0);
    Polygon pm4(Vec3(-1, 2, 2), Vec3(-1, 2, 0), Vec3(-2, 2, 0), Vec3(1,1,0.4), 1, 1.5, 0);
    Polygon pm5(Vec3(-1, 0, 2), Vec3(-2, 0, 2), Vec3(-2, 2, 2), Vec3(1,1,0.4), 1, 1.5, 0);
    Polygon pm6(Vec3(-1, 0, 2), Vec3(-1, 2, 2), Vec3(-2, 2, 2), Vec3(1,1,0.4), 1, 1.5, 0);
    Polygon pm7(Vec3(-1, 0, 2), Vec3(-1, 2, 0), Vec3(-2, 2, 0), Vec3(1,1,0.4), 1, 1.5, 0);
    Polygon pm8(Vec3(-1, 0, 2), Vec3(-2, 0, 2), Vec3(-2, 2, 0), Vec3(1,1,0.4), 1, 1.5, 0);
    std::vector<Polygon> modelPolys = {pm1, pm2, pm3, pm4, pm5, pm6, pm7, pm8};
    PolygonalModel m(modelPolys);

    // _sceneManager->addObject(std::make_shared<PolygonalModel>(m));

    _sceneManager->addObject(std::make_shared<Polygon>(p));
    _sceneManager->addObject(std::make_shared<Polygon>(p1));
    _sceneManager->addObject(std::make_shared<Polygon>(p2));
    _sceneManager->addObject(std::make_shared<Polygon>(p3));

    PolygonalModel model((std::vector<Polygon>()));
    model.load("./DR.obj");
    model.setPosition(Vec3(0,0,0));
    model.setTransparency(1);
    model.setRefractionIndex(1.4);
    model.rotate(Vec3(1,0,0),0);
    model.rotate(Vec3(0,0,1), 0);
    model.rotate(Vec3(0,1,0), 0);
    model.scale(0.15);
    _sceneManager->addObject(std::make_shared<PolygonalModel>(model));


    auto s = std::make_shared<Sphere>(Vec3(4,0,0), 1, Vec3(1,1,0));
    s->_params._emission = {Vec3(1,1,1), 1};
    _sceneManager->addObject(s);

    s = std::make_shared<Sphere>(Vec3(-1.5,0,0), 1, Vec3(1,1,0), 1, 1.2,0);
    // _sceneManager->addObject(s);

    // s = std::make_shared<Sphere>(Vec3(2,2,2), 1, Vec3(1,1,0), 0, 1, 1);
    // _sceneManager->addObject(s);

    _sceneManager->setCamera(std::make_shared<Camera>(Vec3(0,0,-10), Vec3(0,0,1), 1, 45));

    _drawManager->drawScene(_sceneManager->getScene());
}
MainWindow::~MainWindow() {}

void MainWindow::updateMap()
{
    _drawManager->updateSceneMap(_sceneManager->getScene());
}

void MainWindow::renderFrame()
{
    _drawManager->drawScene(_sceneManager->getScene());
}
