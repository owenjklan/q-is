#include <QtWidgets>

#include "qis_widget.hpp"

//#include <QApplication>
//#include <QPushButton>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QISWidget qis;
    qis.show();
    a.setWindowIcon(QIcon("ipinfo.io.png"));
    return a.exec();
}
