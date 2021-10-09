//
// Created by owen on 7/10/21.
//

#include <QJsonDocument>
#include "TabbedResultWidget.hpp"

TabbedResultWidget::TabbedResultWidget(QString *requestedIp, QWidget *parent):
        QTextBrowser(parent)
{
    requestStartTimeMillis = 0;
    requestEndTimeMillis = 0;

    savedToFile = false;
    displayResultsAsJson = false;
    requestErrors = false;

    requestedIp = requestedIp;
    savedFileName = nullptr;
    replyJson = nullptr;
    netReply = nullptr;
}

TabbedResultWidget::~TabbedResultWidget() noexcept {
    // Delete widgets
}

void TabbedResultWidget::setResponseJson(QJsonObject *response) {
    replyJson = response;
}

void TabbedResultWidget::setNetworkReply(QNetworkReply *reply) {
    netReply = reply;
}

void TabbedResultWidget::setDisplayJsonFlag(bool newState) {
    displayResultsAsJson = newState;
    // Don't attempt to render output (that we won't have) if the request
    // resulted in an error.
    if (requestErrors == true) {
        return;
    }
    updateOutput();
}

void TabbedResultWidget::updateOutput() {
    if (displayResultsAsJson == true) {
        clear();
        outputRawJson();
    } else {
        clear();
        outputNicerDetails();
    }
}

void TabbedResultWidget::outputRawJson() {
    QJsonDocument jsonDoc(*replyJson);
    QByteArray jsonRaw = jsonDoc.toJson();
    this->append(jsonRaw);
}

void TabbedResultWidget::outputNicerDetails() {
    QString hostname = replyJson->value("hostname").toString();
    QString region = replyJson->value("region").toString();
    QString city = replyJson->value("city").toString();
    QString country = replyJson->value("country").toString();
    QString org = replyJson->value("org").toString();

    this->append("Hostname: " + hostname);
    this->append("Organisation: " + org);
    this->append("City:     " + city);
    this->append("Region:   " + region);
    this->append("Country:  " + country);
}