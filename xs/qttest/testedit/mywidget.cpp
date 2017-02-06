#include "mywidget.h"
#include <QLineEdit>
#include <QVBoxLayout>

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(new QLineEdit());
    l->addWidget(new QLineEdit());
}


#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MyWidget w;
    w.show();

    return app.exec();
}
