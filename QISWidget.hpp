//
// Created by owen on 4/10/21.
//
#ifndef Q_IS_QISWIDGET_HPP
#define Q_IS_QISWIDGET_HPP

// Sure, these #includes could be less verbose but I am making
// a conscious decision to make them explicit (bloody Python
// programmers :P), laregely for my own educational experience.
//  - oklan.
#include <QtWidgets>

#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUrl>

#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

// This is the declaration of our MainWidget class
// The definition/implementation is in qis_widget.cpp
class QISWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QISWidget(QWidget *parent = 0); //Constructor
    ~QISWidget(); // Destructor

private slots:
    void onLookupButtonReleased();
    void netManagerFinished(QNetworkReply *); // handler for HTTP request response
    void tabCloseRequest(int tabIndex);

private:
    void setupUiAndSignals(QWidget *parent);
    void outputResults(QJsonObject json, QTextBrowser *output);

    QPushButton* lookupButton;
    QLineEdit* ipInput;
    QTabWidget* tabsWidget;

    QNetworkAccessManager *netManager;
    // TODO: Should the below be on the heap instead? Ie: Concurrecy issues??
    QNetworkRequest netRequest;  // On the stack, not the heap
};

#endif //Q_IS_QISWIDGET_HPP
