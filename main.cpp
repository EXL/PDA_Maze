#include "MainWindow.h"

#include <QApplication>

int
main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.setFixedSize(240, 320);
    mainWindow.show();

    return app.exec();
}
