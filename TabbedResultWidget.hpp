//
// Created by owen on 7/10/21.
//

#ifndef Q_IS_TABBEDRESULTWIDGET_HPP
#define Q_IS_TABBEDRESULTWIDGET_HPP

#include <QWidget>
#include <QTextBrowser>

class TabbedResultWidget: public QTextBrowser {
    Q_OBJECT
public:
    TabbedResultWidget();

private:
    int   extraField;
};

#endif //Q_IS_TABBEDRESULTWIDGET_HPP
