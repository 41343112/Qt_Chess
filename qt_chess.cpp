#include "qt_chess.h"
#include "ui_qt_chess.h"

Qt_Chess::Qt_Chess(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Qt_Chess)
{
    ui->setupUi(this);
}

Qt_Chess::~Qt_Chess()
{
    delete ui;
}
