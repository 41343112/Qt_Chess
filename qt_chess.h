#ifndef QT_CHESS_H
#define QT_CHESS_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Qt_Chess;
}
QT_END_NAMESPACE

class Qt_Chess : public QMainWindow
{
    Q_OBJECT

public:
    Qt_Chess(QWidget *parent = nullptr);
    ~Qt_Chess();

private:
    Ui::Qt_Chess *ui;
};
#endif // QT_CHESS_H
