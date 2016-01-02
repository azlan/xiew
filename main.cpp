#include "mainwindow.h"
#include <QApplication>
#include "Configuration.h"

static Configuration* mConfiguration;

int main(int argc, char *argv[])
{
    mConfiguration = new Configuration;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
