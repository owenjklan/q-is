//
// Created by owen on 4/10/21.
//

#include "qis_widget.hpp"

// Constructor for main, "QIS" widget
QISWidget::QISWidget(QWidget *parent) :
        QWidget(parent)
{
    // UI Widget and layout setup
    button_ = new QPushButton(tr("Push Me!"));
    textBrowser_ = new QTextBrowser();
    domainInput_ = new QLineEdit();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    apiPassword = env.value("QIS_API_PASSWORD", "none");
    apiUsername = env.value("QIS_API_USERNAME", "none");
    button_->setFont(QFont("Helvetica", 14, QFont::Bold));
    textBrowser_->setFont(QFont("Courier", 12));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(button_, 0, 0);
    mainLayout->addWidget(domainInput_, 1, 0);
    mainLayout->addWidget(textBrowser_, 2, 0);
    setLayout(mainLayout);
    setWindowTitle(tr("QIS :: WHOIS Desktop Client"));

    // The network stuff
    netManager = new QNetworkAccessManager();
//    netRequest.setRawHeader("apikey", "XkFZg1n93TfFJAyUyYThNUZ9DCz4z1HC");

    // connections
    connect(button_, SIGNAL(released()), this, SLOT(onButtonReleased()));
    connect(netManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(netManagerFinished(QNetworkReply *)));
}

// Destructor
QISWidget::~QISWidget()
{
    delete button_;
    delete textBrowser_;
    delete domainInput_;
    delete netManager;
}

// Handler for button click
void QISWidget::onButtonReleased()
{
    QString domain = QString(domainInput_->text());

    // clear the text in the textBrowser
    textBrowser_->clear();
    textBrowser_->append(tr("Running command:"));
    textBrowser_->append(domain);
    QString apiBase = QString("https://ipinfo.io/%1/json").arg(domain);
//    QString fullUrl = QString("%1?domain=%2").arg(apiBase, domain);
    netRequest.setUrl(QUrl(apiBase));
    netRequest.setRawHeader("Accept", "application/json");
    netRequest.setRawHeader("Authorization", QString("Token token=%1").arg(apiPassword).toUtf8());
    netManager->get(netRequest);
}

void QISWidget::netManagerFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        QString outMessage = QString("Error! %1").arg(reply->errorString());
        textBrowser_->append(outMessage);
        textBrowser_->append(reply->request().url().toDisplayString());
    } else {
        QJsonObject jsonResponse = QJsonDocument::fromJson(reply->readAll()).object();
        QString registrant = jsonResponse["hostname"].toString();
        QString region = jsonResponse["region"].toString();
        textBrowser_->append(registrant);
    }
}