#include "qt_chess.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qt_Chess w;
    w.show();
    return a.exec();
}
