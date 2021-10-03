//
// Created by owen on 4/10/21.
//
#ifndef Q_IS_QIS_WIDGET_HPP
#define Q_IS_QIS_WIDGET_HPP

#include <QWidget>
#include <QProcess>
#include <QStringList>

class QPushButton;
class QTextBrowser;

// This is the declaration of our MainWidget class
// The definition/implementation is in qis_widget.cpp
class QISWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QISWidget(QWidget *parent = 0); //Constructor
    ~QISWidget(); // Destructor

private slots:
    void onButtonReleased();
    void onCaptureProcessOutput(); // Handler for Process output

private:
    QPushButton* button_;
    QTextBrowser* textBrowser_;
    QProcess process_;
    QStringList programArgs_;
};

#endif //Q_IS_QIS_WIDGET_HPP
