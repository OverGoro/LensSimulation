#ifdef TESTING
#ifndef TESTALL_H
#define TESTALL_H

#include "qobject.h"
#include "qtestcase.h"
#include "test_camera.h"
#include "test_polygon.h"
class TestAll : public QObject {
    Q_OBJECT

public:
    TestAll(); // Конструктор
    ~TestAll(); // Деструктор

private slots:
    void runCameraTests();
    void runPolygonTests();
};

#endif // TESTALL_H
#endif
