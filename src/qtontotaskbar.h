#ifndef QTONTOTASKBAR_H
#define QTONTOTASKBAR_H

#include "qtontotaskbar_global.h"

#include <QObject>
#include <QWidget>
#include <qgridlayout.h>
#include "qtimer.h"
#include <QDebug>
#include <QEvent>
#include <windows.h>

class QtOntoTaskbar;

class BackSpaceWidget : public QWidget{
    Q_OBJECT
public:
    explicit BackSpaceWidget(QtOntoTaskbar *p, QWidget *parent = nullptr)
        : QWidget(parent){
        this->p = p;
    }

    QtOntoTaskbar *p;
    ~BackSpaceWidget() override {
    }

    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

};

class QTONTOTASKBAR_EXPORT QtOntoTaskbar : public QObject{
    Q_OBJECT
public:
    explicit QtOntoTaskbar(QObject *parent = nullptr);
    ~QtOntoTaskbar();

    BackSpaceWidget *backspace=nullptr;
    QGridLayout *backspace_layout=nullptr;

    bool eventFilter(QObject*watched, QEvent*event);

    QPair<Qt::Edge,qint32> getTaskbarPosition();
    QPair<HWND, RECT> getTackbar();

    //    void setForegroundColor(const QColor &c);
    void setBackgroundColor(const QColor &c);
    //    QColor foreground_color;
    QColor background_color;
private:
    void init();
    void adjust();
    void upqss_backspace();

    friend class BackSpaceWidget;

signals:
    void edgeChanged(QPair<Qt::Edge,qint32> edge);
};

#endif // QTONTOTASKBAR_H
