//
// Created by owen on 4/10/21.
//

#include "qis_widget.hpp"

// Constructor for main, "QIS" widget
QISWidget::QISWidget(QWidget *parent) :
        QWidget(parent)
{
    // UI Widget and layout setup
    button_ = new QPushButton(tr("GeoIP Lookup..."));
    domainInput_ = new QLineEdit();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    apiPassword = env.value("QIS_API_PASSWORD", "none");
    apiUsername = env.value("QIS_API_USERNAME", "none");
    button_->setFont(QFont("Helvetica", 14, QFont::Bold));

    tabsWidget = new QTabWidget(parent);
    tabsWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    tabsWidget->setMinimumSize(720, 480);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(domainInput_, 0, 0);
    mainLayout->addWidget(button_, 0, 1);
    mainLayout->addWidget(tabsWidget, 1, 0, 2, 1, Qt::AlignCenter);
    setLayout(mainLayout);
    setWindowTitle(tr("QIS :: WHOIS Desktop Client"));

    // The network stuff
    netManager = new QNetworkAccessManager();

    // connections
    connect(button_, SIGNAL(released()), this, SLOT(onButtonReleased()));
    connect(netManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(netManagerFinished(QNetworkReply *)));
}

// Destructor
QISWidget::~QISWidget()
{
    delete button_;
    delete tabsWidget;
    delete domainInput_;
    delete netManager;
}

// Handler for button click
void QISWidget::onButtonReleased()
{
    QString domain = QString(domainInput_->text());

    if (domain.length() < 7) {  // Really lazy assumption that a valid dotted-quad IPv4 address is at least 7 chars long
        QMessageBox msgBox;
        msgBox.setText("You must provide a valid IPv4 address!");
        msgBox.exec();
        return;
    }

    QTextBrowser *newTabTextBrowser = new QTextBrowser();
    newTabTextBrowser->setFont(QFont("courier", 12));
    newTabTextBrowser->setOpenExternalLinks(true);
    int newTabIndex = tabsWidget->addTab(newTabTextBrowser, domain);
    // clear the text in the textBrowser
    newTabTextBrowser->clear();
    QString apiBase = QString("https://ipinfo.io/%1/json").arg(domain);
    QString webBase = QString("<a href='https://ipinfo.io/%1'>%1</a>").arg(domain);

    newTabTextBrowser->append(tr("Making request: ") + webBase);
    newTabTextBrowser->append(domain);
//    QString fullUrl = QString("%1?domain=%2").arg(apiBase, domain);

    netRequest.setUrl(QUrl(apiBase));
    netRequest.setRawHeader("Accept", "application/json");
    netRequest.setRawHeader("Authorization", QString("Token token=%1").arg(apiPassword).toUtf8());
    netRequest.setOriginatingObject(newTabTextBrowser);
    netManager->get(netRequest);
}

void QISWidget::outputResults(QJsonObject json, QTextBrowser *output) {
    QString hostname = json["hostname"].toString();
    QString region = json["region"].toString();
    QString city = json["city"].toString();
    QString country = json["country"].toString();
    QString org = json["org"].toString();

    output->append("Hostname: " + hostname);
    output->append("Organisation: " + org);
    output->append("City:     " + city);
    output->append("Region:   " + region);
    output->append("Country:  " + country);
}

void QISWidget::netManagerFinished(QNetworkReply *reply) {
    QTextBrowser *requestOutput;
    QNetworkRequest originalRequest;

    originalRequest = reply->request();
    requestOutput = dynamic_cast<QTextBrowser *>(originalRequest.originatingObject());

    requestOutput->append("Response received");

    if (reply->error() != QNetworkReply::NoError) {
        QString outMessage = QString("Error! %1").arg(reply->errorString());
        requestOutput->append(outMessage);
        requestOutput->append(reply->request().url().toDisplayString());
    } else {
        QJsonObject jsonResponse = QJsonDocument::fromJson(reply->readAll()).object();
        outputResults(jsonResponse, requestOutput);
    }
}