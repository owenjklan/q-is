//
// Created by owen on 4/10/21.
//

#include "QISWidget.hpp"
#include "TabbedResultWidget.hpp"

#include <QDebug>

// Constructor for main, "QIS" widget
QISWidget::QISWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUiAndSignals(parent);

    // The network stuff
    netManager = new QNetworkAccessManager();

    // When a HTTP request has finished and we have a reponse
    connect(netManager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(netManagerFinished(QNetworkReply *)));
}

// Destructor
QISWidget::~QISWidget()
{
    delete lookupButton;
    delete tabsWidget;
    delete ipInput;
    delete netManager;
}

// Our own UI setup function. We build our UI from scratch
void QISWidget::setupUiAndSignals(QWidget *parent) {
    // Create and setup all our widgets
    // "input widgets"
    lookupButton = new QPushButton(tr("GeoIP Lookup"));
    lookupButton->setFont(QFont("Helvetica", 10, QFont::Bold));
    lookupButton->setMaximumWidth(150);
    lookupButton->setToolTip(tr("Perform a GeoIP lookup against ipinfo.io."));
    lookupButton->setToolTipDuration(2000);

    saveButton = new QPushButton(tr("Save"));
    saveButton->setMaximumWidth(100);
    saveButton->setToolTip(tr(
            "Save the current output to disk. "
            "Note: This will save EXACTLY what is visible in the tab,"
            " do NOT assume it is always JSON!"));
    saveButton->setToolTipDuration(2000);
    saveButton->setDisabled(true);

    ipInput = new QLineEdit();
    ipInput->setMaximumWidth(200);
    ipInput->setPlaceholderText("1.2.3.4");
    ipInput->setToolTip(tr("Enter an IPv4 address in dotted-quad notation."));
    ipInput->setToolTipDuration(2000);

    // "output & control widgets"
    tabsWidget = new QTabWidget(parent);
    tabsWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    tabsWidget->setMinimumSize(720, 480);
    tabsWidget->setTabsClosable(true);

    displayJsonCheck = new QCheckBox("Display Raw JSON");
    displayJsonCheck->setDisabled(true);

    // Layout all the widgets
    QGridLayout *mainLayout = new QGridLayout;
    QVBoxLayout *controlsVLayout = new QVBoxLayout;
    QHBoxLayout *inputsHLayout = new QHBoxLayout;
    QHBoxLayout *resultsHLayout = new QHBoxLayout;

    controlsVLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    controlsVLayout->addWidget(saveButton);
    controlsVLayout->addWidget(displayJsonCheck);

    inputsHLayout->addWidget(ipInput);
    inputsHLayout->addWidget(lookupButton);

    resultsHLayout->addWidget(tabsWidget);
    resultsHLayout->addLayout(controlsVLayout);

    mainLayout->addLayout(inputsHLayout, 0, 0);
    mainLayout->addLayout(resultsHLayout, 1, 0);

    setLayout(mainLayout);
    setWindowTitle(tr("ipinfo.io - Unofficial Desktop Client"));

    // Connect signals for UI widgets
    connect(lookupButton, SIGNAL(released()),
            this, SLOT(onLookupButtonReleased()));
    connect(saveButton, SIGNAL(released()),
            this, SLOT(onSaveButtonReleased()));
    connect(ipInput, SIGNAL(returnPressed()),
            this, SLOT(onLookupButtonReleased()));
    connect(tabsWidget, SIGNAL(tabCloseRequested(int)),
            this, SLOT(tabCloseRequest(int)));
    connect(tabsWidget, SIGNAL(currentChanged(int)),
            this, SLOT(tabChanged(int)));
    connect(displayJsonCheck, SIGNAL(stateChanged(int)),
            this, SLOT(onDisplayJsonChange(int)));
}

void QISWidget::onSaveButtonReleased() {
    TabbedResultWidget *currentResult = dynamic_cast<TabbedResultWidget *>(tabsWidget->currentWidget());
    QString *extension = new QString((currentResult->displayResultsAsJson) ? "json" : "txt");
    QString *ip = currentResult->requestedIp;
    QString suggestedFilename;
    suggestedFilename = *ip + QString(".") + *extension;
    QString filename = QFileDialog::getSaveFileName(
        this,
        tr("Save tab output"),
        suggestedFilename
    );

    QMessageBox msgbox;
    msgbox.setText("Filename: " + filename);
    msgbox.exec();
}

// Handler for click of the "GeoIP Lookup" button.
void QISWidget::onLookupButtonReleased()
{
    // Creates a new text browser and makes a network request to
    // ipinfo.io for the target IP address. The new text browser
    // is set as the Originating Object on the network request,
    // this way, in the reply handler, we know what widget to
    // output to.
    QString *requestedIp4Addr = new QString(ipInput->text());
    if (requestedIp4Addr->length() < 7) {  // Really lazy assumption that a valid dotted-quad IPv4 address is at least 7 chars long
        QMessageBox msgBox;
        msgBox.setText("You must provide a valid IPv4 address!");
        msgBox.exec();
        delete requestedIp4Addr;
        return;
    }

    // Setup results tab widget
    TabbedResultWidget *newResultTab = new TabbedResultWidget(requestedIp4Addr);
    newResultTab->setFont(QFont("courier", 12));
    newResultTab->setOpenExternalLinks(true);

    int newIndex = tabsWidget->addTab(newResultTab, *requestedIp4Addr);
    tabsWidget->setCurrentIndex(newIndex);

    // Setup HTTPS request for IPInfo. We expect JSON responses
    QString apiBase = QString("https://ipinfo.io/%1/json").arg(*requestedIp4Addr);
    QString webBase = QString("<a href='https://ipinfo.io/%1'>%1</a>").arg(*requestedIp4Addr);

    newResultTab->append(tr("Making request: ") + webBase);
    QDateTime date;
    newResultTab->requestStartTimeMillis = date.toMSecsSinceEpoch();

    netRequest.setUrl(QUrl(apiBase));
    netRequest.setRawHeader("Accept", "application/json");
    netRequest.setOriginatingObject(newResultTab);
    netManager->get(netRequest);

    // Note: We only enable the "Display Raw JSON" checkbox for a
    // given tab, AFTER it's associated network response comes back
    // WITHOUT any error conditions!
}

void QISWidget::onDisplayJsonChange(int newState) {
    TabbedResultWidget *currentResults = dynamic_cast<TabbedResultWidget *>(tabsWidget->currentWidget());

    if (newState == Qt::Checked) {
        currentResults->setDisplayJsonFlag(true);
    } else {
        currentResults->setDisplayJsonFlag(false);
    }
}

// Slot function for pressing the little "close" button on tabs
void QISWidget::tabCloseRequest(int tabIndex) {
    QWidget *widgetToDelete = tabsWidget->widget(tabIndex);
    tabsWidget->removeTab(tabIndex);
    delete widgetToDelete;

    // If there are no tabs left, we should disable the "Display JSON"
    // checkbox
    if (tabsWidget->count() == 0) {
        disableControls();
    }
}

void QISWidget::tabChanged(int tabIndex) {
    TabbedResultWidget *currentResult = dynamic_cast<TabbedResultWidget *>(tabsWidget->currentWidget());

    // This condition can show up if we've closed the final tab
    if (currentResult == nullptr) {
    // The commented stuff was just to prove a point. Could probably go.
//        QMessageBox msgBox;
//        msgBox.setText("NULL crash avoided");
//        msgBox.exec();
        return;
    }
    if (currentResult->displayResultsAsJson == true) {
        displayJsonCheck->setCheckState(Qt::Checked);
    } else {
        displayJsonCheck->setCheckState(Qt::Unchecked);
    }

    if (currentResult->requestErrors == true) {
        disableControls();
    } else {
        enableControls();
    }
}

// Slot function for receiving reponses to HTTP requests we made.
// TODO: This needs serious thought regarding possible concurrency issues
//       when multiple requests are in flight.
void QISWidget::netManagerFinished(QNetworkReply *reply) {
    TabbedResultWidget *requestOutput;
    QNetworkRequest originalRequest;

    originalRequest = reply->request();
    requestOutput = dynamic_cast<TabbedResultWidget *>(originalRequest.originatingObject());
    QDateTime date;
    requestOutput->requestEndTimeMillis = date.toMSecsSinceEpoch();
    requestOutput->requestDurationMillis = requestOutput->requestEndTimeMillis - requestOutput->requestStartTimeMillis;

    requestOutput->append(
            QString("Response received after %1 milliseconds\n").arg(
                    requestOutput->requestDurationMillis
                )
            );

    if (reply->error() != QNetworkReply::NoError) {
        QString outMessage = QString("Error! %1").arg(reply->errorString());
        requestOutput->append(outMessage);
        requestOutput->append("URL Requested:");
        requestOutput->append(reply->request().url().toDisplayString());
        requestOutput->requestErrors = true;
        disableControls();
    } else {
        QJsonObject jsonResponse = QJsonDocument::fromJson(reply->readAll()).object();
        requestOutput->setResponseJson(new QJsonObject(jsonResponse));
        requestOutput->requestErrors = false;
        requestOutput->updateOutput();
        enableControls();
    }
}

// The following en/disableControls() methods get all of the "common controls"
// from the right-side sub-panel.

// Called if we:
// - switch to a tab that was an error,
// - have close the last tab
void QISWidget::disableControls() {
    displayJsonCheck->setDisabled(true);
    saveButton->setDisabled(true);
}

// Called if we:
// - Have a reponse that is not an error
// - Switch to a tab that does not have an error
void QISWidget::enableControls() {
    displayJsonCheck->setEnabled(true);
    saveButton->setEnabled(true);
}