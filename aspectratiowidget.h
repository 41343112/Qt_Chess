#ifndef ASPECTRATIOWIDGET_H
#define ASPECTRATIOWIDGET_H

#include <QWidget>

class AspectRatioWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AspectRatioWidget(QWidget *childWidget = nullptr, QWidget *parent = nullptr);
    bool hasHeightForWidth() const override { return true; }
    int heightForWidth(int w) const override { return w; }
    QSize minimumSizeHint() const override { return QSize(64, 64); }
    QSize sizeHint() const override { return QSize(400, 400); }
    void setContentWidget(QWidget *w);
    
protected:
    void resizeEvent(QResizeEvent *event) override;
    
private:
    QWidget *m_content = nullptr;
};

#endif // ASPECTRATIOWIDGET_H
