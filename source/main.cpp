#include "mainwindow.hpp"

#include <QApplication>

//#include "TestCases_LineMethods.hpp"
//
// int main(int argc, char* argv[]) {
// 	testing::InitGoogleTest(&argc, argv);
// 	return RUN_ALL_TESTS();
// }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
