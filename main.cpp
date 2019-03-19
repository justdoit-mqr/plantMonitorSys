#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    MainWidget w;
    //w.resize(800,480);
    //w.show();
    w.showFullScreen();

    return a.exec();
}
