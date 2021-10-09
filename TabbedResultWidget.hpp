//
// Created by owen on 7/10/21.
//

#ifndef Q_IS_TABBEDRESULTWIDGET_HPP
#define Q_IS_TABBEDRESULTWIDGET_HPP

#include <QWidget>
#include <QTextBrowser>

#include <QJsonObject>
#include <QNetworkReply>

class TabbedResultWidget: public QTextBrowser {
Q_OBJECT
public:
    explicit TabbedResultWidget(QString *ip, QWidget *parent = 0); //Constructor
    ~TabbedResultWidget(); // Destructor

    void setNetworkReply(QNetworkReply *reply);
    void setResponseJson(QJsonObject *response);
    void setDisplayJsonFlag(bool newState);

    void updateOutput();

    qint64 requestStartTimeMillis;
    qint64 requestEndTimeMillis;
    qint64 requestDurationMillis;

    bool displayResultsAsJson;
    bool savedToFile;
    bool requestErrors;
    QString *savedFileName;
    QString *requestedIp;

    QJsonObject *replyJson;
    QNetworkReply *netReply;

private slots:

private:
    void outputRawJson();
    void outputNicerDetails();
};

#endif //Q_IS_TABBEDRESULTWIDGET_HPP
