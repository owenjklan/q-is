//
// Created by owen on 4/10/21.
//
#ifndef Q_IS_QIS_WIDGET_HPP
#define Q_IS_QIS_WIDGET_HPP

#include <QFont>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QString>
#include <QStringList>
#include <QtWidgets>

class QPushButton;
class QTextBrowser;
class QLineEdit;
class QProcessEnvironment;
class QNetworkAccessManager;
class QNetworkRequest;

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
    void netManagerFinished(QNetworkReply *); // handler for HTTP request response

private:
    QPushButton* button_;
    QTextBrowser* textBrowser_;
    QLineEdit* domainInput_;
    QNetworkAccessManager *netManager;
    QNetworkRequest netRequest;  // On the stack, not the heap

    QString apiUsername;
    QString apiPassword;
};

#endif //Q_IS_QIS_WIDGET_HPP
