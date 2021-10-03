//
// Created by owen on 4/10/21.
//

#include <QFont>
#include <QStringList>
#include <QtWidgets>
#include "qis_widget.hpp"

// Constructor for main, "QIS" widget
QISWidget::QISWidget(QWidget *parent) :
        QWidget(parent)
{
    button_ = new QPushButton(tr("Push Me!"));
    textBrowser_ = new QTextBrowser();
    programArgs_ = QStringList({"-la", "/home/owen"});

    button_->setFont(QFont("Helvetica", 14, QFont::Bold));
    textBrowser_->setFont(QFont("Courier", 12));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(button_,0,0);
    mainLayout->addWidget(textBrowser_,1,0);
    setLayout(mainLayout);
    setWindowTitle(tr("QIS :: WHOIS Desktop Client"));

    connect(button_, SIGNAL(released()), this, SLOT(onButtonReleased()));
    connect(&process_, SIGNAL(readyReadStandardOutput()), this, SLOT(onCaptureProcessOutput()));
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

    // Set up our process to write to stdout and run our command
    process_.setCurrentWriteChannel(QProcess::StandardOutput); // Set the write channel
    process_.start("/usr/bin/ls", programArgs_, QIODevice::ReadOnly); // Start the program
}

// This is called whenever the QProcess::readyReadStandardOutput() signal is received
void QISWidget::onCaptureProcessOutput()
{
    // Determine whether the object that sent the signal was a pointer to a process
    QProcess* process = qobject_cast<QProcess*>(sender());
    // If so, append the output to the textbrowser
    if (process)
        textBrowser_->append(process->readAllStandardOutput());
}