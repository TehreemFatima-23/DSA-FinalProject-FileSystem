#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("File System Simulator");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("DSA Semester Project");

    MainWindow window;
    window.show();

    return app.exec();
}