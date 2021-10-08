//
// Created by owen on 4/10/21.
//

#include "qis_widget.hpp"

#include <QDebug>

// Constructor for main, "QIS" widget
QISWidget::QISWidget(QWidget *parent) :
        QWidget(parent)
{
    // UI Widget and layout setup
    lookupButton = new QPushButton(tr("GeoIP Lookup"));
    ipInput = new QLineEdit();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    apiPassword = env.value("QIS_API_PASSWORD", "none");
    apiUsername = env.value("QIS_API_USERNAME", "none");
    lookupButton->setFont(QFont("Helvetica", 10, QFont::Bold));

    ipInput->setMaximumWidth(200);
    ipInput->setPlaceholderText("1.2.3.4");
    lookupButton->setMaximumWidth(150);

    tabsWidget = new QTabWidget(parent);
    tabsWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    tabsWidget->setMinimumSize(720, 480);
    tabsWidget->setTabsClosable(true);

    QGridLayout *mainLayout = new QGridLayout;
    QHBoxLayout *topHLayout = new QHBoxLayout;
    topHLayout->addWidget(ipInput);
    topHLayout->addWidget(lookupButton);
    mainLayout->addLayout(topHLayout, 0, 0);
    mainLayout->addWidget(tabsWidget, 1, 0);
    setLayout(mainLayout);
    setWindowTitle(tr("ipinfo.io - Unofficial Desktop Client"));

    // The network stuff
    netManager = new QNetworkAccessManager();

    // connections
    connect(lookupButton, SIGNAL(released()), this, SLOT(onButtonReleased()));
    connect(netManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(netManagerFinished(QNetworkReply *)));
    connect(tabsWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequest(int)));
}

// Destructor
QISWidget::~QISWidget()
{
    delete lookupButton;
    delete tabsWidget;
    delete ipInput;
    delete netManager;
}

// Handler for button click
void QISWidget::onButtonReleased()
{
    // Creates a new text browser and makes a network request to
    // ipinfo.io for the target IP address. The new text browser
    // is set as the Originating Object on the network request,
    // this way, in the reply handler, we know what widget to
    // output to.
    QTextBrowser *newTabTextBrowser = new QTextBrowser();
    newTabTextBrowser->setFont(QFont("courier", 12));
    newTabTextBrowser->setOpenExternalLinks(true);

    QString domain = QString(ipInput->text());
    if (domain.length() < 7) {  // Really lazy assumption that a valid dotted-quad IPv4 address is at least 7 chars long
        QMessageBox msgBox;
        msgBox.setText("You must provide a valid IPv4 address!");
        msgBox.exec();
        return;
    }

    int newTabIndex = tabsWidget->addTab(newTabTextBrowser, domain);

    QString apiBase = QString("https://ipinfo.io/%1/json").arg(domain);
    QString webBase = QString("<a href='https://ipinfo.io/%1'>%1</a>").arg(domain);

    newTabTextBrowser->append(tr("Making request: ") + webBase);

    netRequest.setUrl(QUrl(apiBase));
    netRequest.setRawHeader("Accept", "application/json");
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

void QISWidget::tabCloseRequest(int tabIndex) {
//    qDebug() << "Closing TabIndex==" << tabIndex << Qt::endl;
    QWidget *widgetToDelete = tabsWidget->widget(tabIndex);
    tabsWidget->removeTab(tabIndex);
    delete widgetToDelete;
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
        requestOutput->append("URL Requested:");
        requestOutput->append(reply->request().url().toDisplayString());
    } else {
        QJsonObject jsonResponse = QJsonDocument::fromJson(reply->readAll()).object();
        outputResults(jsonResponse, requestOutput);
    }
}