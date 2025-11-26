// #include <QTest>
// #include "test_camera.cpp"
// int main(int argc, char *argv[])
// {
//     QCoreApplication a(argc, argv);
//     TestCamera tc;
//     QTest::qExec(&tc, argc, argv);
//     return 0;
// }
#include <QtTest/QtTest>
#include "test_camera.cpp" // или используйте заголовочный файл с определениями тестов

#ifdef TESTING
QTEST_MAIN(TestAll)
#endif
