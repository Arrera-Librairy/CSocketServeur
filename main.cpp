#include <QCoreApplication>
#include "csocketserveur.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CSocketServeur serveur = new CSocketServeur();
    serveur.startServer(1012);

    return a.exec();
}
