#include "mainwindow.h"
#include <QApplication>
#include "Configuration.h"
#include "capstone_wrapper.h"

static Configuration* mConfiguration;

int main(int argc, char *argv[])
{
    mConfiguration = new Configuration;
    Capstone::GlobalInitialize();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
