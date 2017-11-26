#include <QCoreApplication>
#include <QSettings>
#include <QDir>

#include "host.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSettings settings(QDir::currentPath().append("/config.ini"), QSettings::IniFormat);
    settings.beginGroup("Configuration parameters");
        QString address            = settings.value("address").toString();
        int port                   = settings.value("port").toInt();
        QString dataFolderLocation = settings.value("dataFolderLocation").toString();
        int maxNumFiles            = settings.value("maxNumFiles").toInt();
        QString mediaLocation      = settings.value("mediaLocation").toString();
    settings.endGroup();

    Host host(address, port, dataFolderLocation, maxNumFiles);
    host.receive(mediaLocation);

    return a.exec();
}
