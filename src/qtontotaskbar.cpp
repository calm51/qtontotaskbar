#include "qtontotaskbar.h"

#include <QCoreApplication>

bool BackSpaceWidget::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    MSG* msg = reinterpret_cast<MSG*>(message);
    //    qDebug()<<msg->message;
    if(msg->message==26){
        APPBARDATA abd;
        abd.cbSize = sizeof(APPBARDATA);
        if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd)){
            //            RECT taskbarRect = abd.rc;
            //            UINT taskbarEdge = abd.uEdge;
            //            qDebug() << "Taskbar Rect: left=" << taskbarRect.left
            //                     << ", top=" << taskbarRect.top
            //                     << ", right=" << taskbarRect.right
            //                     << ", bottom=" << taskbarRect.bottom;
            //            qDebug() << "Taskbar Edge:" << taskbarEdge;

            const auto &edge = this->p->getTaskbarPosition();
            emit this->p->edgeChanged(edge);
            this->p->adjust();

        }
    }
    else if(msg->message==136){this->p->adjust();}
    else if(msg->message==133){this->p->adjust();}
    else if(msg->message==20){this->p->adjust();}
    else if(msg->message==15){this->p->adjust();}

    //    else{qDebug()<<msg->message;}

    return false;
}


QtOntoTaskbar::QtOntoTaskbar(QObject *parent)
    : QObject{parent},backspace(new BackSpaceWidget(this)),backspace_layout(new QGridLayout(backspace)),background_color{0,0,0,0}
{
    backspace_layout->setContentsMargins(0,0,0,0);
    backspace_layout->setHorizontalSpacing(0);
    backspace_layout->setVerticalSpacing(0);
    this->init();
    backspace->installEventFilter(this);

}


QtOntoTaskbar::~QtOntoTaskbar(){
    backspace->hide();
    SetParent((HWND)backspace->winId(), NULL);
    backspace->hide();
    //    if(backspace_layout){
    //        delete backspace_layout;
    //        backspace_layout = nullptr;
    //    }
    //    if(backspace){
    //        delete backspace;
    //        backspace = nullptr;
    //    }
    backspace->deleteLater();
    backspace = nullptr;

    QCoreApplication::processEvents();

    const QPair<HWND, RECT> &taskbar = getTackbar();
    HWND hReBar = FindWindowEx(taskbar.first,NULL,"ReBarWindow32",NULL);
    RECT rcReBar; GetWindowRect(hReBar,&rcReBar);
    HWND hMin = FindWindowEx(hReBar,NULL,"MSTaskSwWClass",NULL);
    RECT rcMin; GetWindowRect(hMin,&rcMin);
    MoveWindow(hMin, 0, 0, rcReBar.right - rcReBar.left, rcReBar.bottom - rcReBar.top, TRUE);




}

bool QtOntoTaskbar::eventFilter(QObject *watched, QEvent *event){
    if (watched==backspace){
        //        qDebug()<<"backspace event: "<<event->type();
        if(event->type()==QEvent::LayoutRequest){
            adjust();
        }
    }
    return QObject::eventFilter(watched,event);

}

QPair<HWND, RECT> QtOntoTaskbar::getTackbar(){
    QPair<HWND, RECT> result;
    result.first = FindWindowEx(NULL,NULL,"Shell_TrayWnd",NULL); // FindWindow("Shell_traywnd",NULL);
    GetWindowRect(result.first, &result.second);
    return result;
}

//void QtOntoTaskbar::setForegroundColor(const QColor &c){
//    this->foreground_color = c;
//    upqss_backspace();
//}
void QtOntoTaskbar::setBackgroundColor(const QColor &c){
    this->background_color = c;
    upqss_backspace();
}

void QtOntoTaskbar::upqss_backspace(){
    if (!backspace){return;}
    backspace->setStyleSheet(QString("BackSpaceWidget#backspace{background:%2;}") // *{color:%1;}
                             //                             .arg(QString("rgba(%1,%2,%3,%4)").arg(foreground_color.red()).arg(foreground_color.green()).arg(foreground_color.blue()).arg(foreground_color.alpha()))
                             .arg(QString("rgba(%1,%2,%3,%4)").arg(background_color.red()).arg(background_color.green()).arg(background_color.blue()).arg(background_color.alpha()))
                             );
    // qDebug()<<backspace->styleSheet();
}



QPair<Qt::Edge,qint32> QtOntoTaskbar::getTaskbarPosition() {
    QPair<Qt::Edge,qint32> result;
    const QPair<HWND, RECT> &taskbar = getTackbar();
    result.second = taskbar.second.bottom-taskbar.second.top;
    APPBARDATA abd = { 0 };
    abd.cbSize = sizeof(APPBARDATA);
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) {
        UINT taskbarEdge = abd.uEdge;
        switch (taskbarEdge) {
        case ABE_LEFT:
            result.first = Qt::LeftEdge; // 任务栏在左边
            result.second = taskbar.second.right-taskbar.second.left;
            return result;
        case ABE_TOP:
            result.first = Qt::TopEdge;  // 任务栏在顶部
            return result;
        case ABE_RIGHT:
            result.first = Qt::RightEdge; // 任务栏在右边
            result.second = taskbar.second.right-taskbar.second.left;
            return result;
        case ABE_BOTTOM:
            result.first = Qt::BottomEdge; // 任务栏在底部
            return result;
        default:
            result.first = Qt::BottomEdge;
            return result;
        }
    }
    result.first = Qt::BottomEdge;
    return result;

}


void QtOntoTaskbar::init(){
    if (!backspace){return;}
    backspace->setObjectName("backspace");
    upqss_backspace();
    backspace->setWindowFlag(Qt::FramelessWindowHint);
    backspace->setWindowFlag(Qt::WindowStaysOnTopHint);
    //    backspace->show();

    const QPair<HWND, RECT> &taskbar = getTackbar();
    const auto &edge = this->getTaskbarPosition();
    if(edge.first==Qt::TopEdge || edge.first==Qt::BottomEdge){
        backspace->setFixedHeight(edge.second);
        backspace->setFixedWidth(backspace->sizeHint().width());
        backspace->setFixedWidth(4);
    }else if(edge.first==Qt::LeftEdge || edge.first==Qt::RightEdge){
        backspace->setFixedWidth(edge.second);
        backspace->setFixedHeight(backspace->sizeHint().height());
        backspace->setFixedHeight(4);
    }

    HWND hReBar = FindWindowEx(taskbar.first,NULL,"ReBarWindow32",NULL);
    RECT rcReBar; GetWindowRect(hReBar,&rcReBar);
    //    qDebug()<<"rcReBar: "<<rcReBar.left<<rcReBar.top<<rcReBar.right<<rcReBar.bottom;
    HWND hMin = FindWindowEx(hReBar,NULL,"MSTaskSwWClass",NULL);
    RECT rcMin; GetWindowRect(hMin,&rcMin);
    //    qDebug()<<"rcMin: "<<rcMin.left<<rcMin.top<<rcMin.right<<rcMin.bottom;

    int windowX = 0; int windowY = 0;
    if(edge.first==Qt::TopEdge || edge.first==Qt::BottomEdge){
        MoveWindow(hMin, 0, 0, rcReBar.right - rcReBar.left - backspace->width(), rcReBar.bottom - rcReBar.top, TRUE);
        windowX = rcReBar.right-rcReBar.left - backspace->width();
        windowY = (rcReBar.bottom - rcReBar.top - backspace->height())/2;
    }else if(edge.first==Qt::LeftEdge){
        MoveWindow(hMin, 0, 0, rcReBar.right-rcReBar.left, rcReBar.bottom - rcReBar.top-backspace->height(), TRUE);
        windowX = rcReBar.left;
        windowY = rcReBar.bottom - rcReBar.top-backspace->height();
    }else if(edge.first==Qt::RightEdge){
        MoveWindow(hMin, 0, 0, rcReBar.right-rcReBar.left, rcReBar.bottom - rcReBar.top-backspace->height(), TRUE);
        windowX = 0;
        windowY = rcReBar.bottom - rcReBar.top-backspace->height();
    }
    SetParent((HWND)backspace->winId(),hReBar);
    MoveWindow((HWND)backspace->winId(),windowX,windowY,backspace->width(),backspace->height(),TRUE);

}


void QtOntoTaskbar::adjust(){
    if (!backspace){return;}
    //    return;
    //    qDebug()<<"adjust: "<<backspace_layout->sizeHint()<<backspace->sizeHint();
    QSize size = backspace->sizeHint();
    const QPair<HWND, RECT> &taskbar = getTackbar();
    const auto &edge = this->getTaskbarPosition();

    if(edge.first==Qt::TopEdge || edge.first==Qt::BottomEdge){
        size.setHeight(edge.second);
    }else if(edge.first==Qt::LeftEdge || edge.first==Qt::RightEdge){
        size.setWidth(edge.second);
    }

    HWND hReBar = FindWindowEx(taskbar.first,NULL,"ReBarWindow32",NULL);
    RECT rcReBar; GetWindowRect(hReBar,&rcReBar);
    //    qDebug()<<"rcReBar: "<<rcReBar.left<<rcReBar.top<<rcReBar.right<<rcReBar.bottom;
    HWND hMin = FindWindowEx(hReBar,NULL,"MSTaskSwWClass",NULL);
    RECT rcMin; GetWindowRect(hMin,&rcMin);
    //    qDebug()<<"rcMin: "<<rcMin.left<<rcMin.top<<rcMin.right<<rcMin.bottom;

    int windowX = 0; int windowY = 0;
    if(edge.first==Qt::TopEdge || edge.first==Qt::BottomEdge){
        MoveWindow(hMin, 0, 0, rcReBar.right - rcReBar.left - size.width(), rcReBar.bottom - rcReBar.top, TRUE);
        windowX = rcReBar.right-rcReBar.left - size.width();
        windowY = (rcReBar.bottom - rcReBar.top - size.height())/2;
    }else if(edge.first==Qt::LeftEdge){
        MoveWindow(hMin, 0, 0, rcReBar.right-rcReBar.left, rcReBar.bottom - rcReBar.top-size.height(), TRUE);
        windowX = rcReBar.left;
        windowY = rcReBar.bottom - rcReBar.top-size.height();
    }else if(edge.first==Qt::RightEdge){
        MoveWindow(hMin, 0, 0, rcReBar.right-rcReBar.left, rcReBar.bottom - rcReBar.top-size.height(), TRUE);
        windowX = 0;
        windowY = rcReBar.bottom - rcReBar.top-size.height();
    }
    MoveWindow((HWND)backspace->winId(),windowX,windowY,size.width(),size.height(),TRUE);

    //    qDebug()<<"changed: "<<backspace_layout->sizeHint()<<backspace->sizeHint();
    backspace->show();

    //    SetParent((HWND)pb->winId(), NULL);
    //    QTimer::singleShot(1000,[=](){
    //        SetParent((HWND)backspace->winId(), NULL);
    //        backspace->setFixedWidth(10);
    //        SetParent((HWND)backspace->winId(),hReBar);
    //        MoveWindow((HWND)backspace->winId(),windowX,windowY,backspace->width(),backspace->height(),TRUE);
    //    });
}



