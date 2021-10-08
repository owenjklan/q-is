#include <QtWidgets>

#include "QISWidget.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QISWidget qis;
    qis.show();
    a.setWindowIcon(QIcon("ipinfo.io.png"));
    return a.exec();
}
