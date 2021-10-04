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

    button_->setFont(QFont("Helvetica", 14, QFont::Bold));
    textBrowser_->setFont(QFont("Courier", 12));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(button_,0,0);
    mainLayout->addWidget(textBrowser_,1,0);
    setLayout(mainLayout);
    setWindowTitle(tr("QIS :: WHOIS Desktop Client"));

    // The network stuff
    netManager = new QNetworkAccessManager();
    netRequest.setUrl(QUrl("https://www.example.com/"));

    // connections
    connect(button_, SIGNAL(released()), this, SLOT(onButtonReleased()));
    connect(netManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(netManagerFinished(QNetworkReply *)));
}

// Destructor
QISWidget::~QISWidget()
{
    delete button_;
    delete textBrowser_;
}

// Handler for button click
void QISWidget::onButtonReleased()
{
    // clear the text in the textBrowser
    textBrowser_->clear();
    textBrowser_->append(tr("Running command:"));

    netRequest.setUrl(QUrl("https://www.example.com/index.html"));
    netManager->get(netRequest);
}

void QISWidget::netManagerFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        QString outMessage = QString("Error! %1").arg(reply->error());
        textBrowser_->append(outMessage);

        textBrowser_->append(reply->request().url().toDisplayString());
    } else {
        textBrowser_->append(reply->readAll());
    }
}