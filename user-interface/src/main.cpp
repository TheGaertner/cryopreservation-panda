#include "user-interface.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // a.setWindowIcon(QIcon("./symbols/app.png"));
    UserInterface w = UserInterface(nullptr,argv[0]);
    w.show();
    return a.exec();
}
