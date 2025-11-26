#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "renderingwidget.h"
#include "scenewidget.h"

#include "scenemanager.h"
#include "drawmanager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected slots:
    void updateMap();
    void renderFrame();
protected:
    RenderingWidget *_renderingWidget;
    SceneWidget *_sceneWidget;

    std::shared_ptr<SceneManager>  _sceneManager;
    std::shared_ptr<DrawManager> _drawManager;
};
#endif // MAINWINDOW_H
