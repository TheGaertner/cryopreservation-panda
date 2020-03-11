#include "user-interface.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConfigHandler::setConfig(argv[0],true);
    UserInterface w = UserInterface(nullptr,argv[0]);
    w.show();
    return a.exec();
}
