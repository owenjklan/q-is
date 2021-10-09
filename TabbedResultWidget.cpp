//
// Created by owen on 7/10/21.
//

#include "TabbedResultWidget.hpp"

TabbedResultWidget::TabbedResultWidget(QString *requestedIp, QWidget *parent):
        QTextBrowser(parent)
{
    requestStartTimeMillis = 0;
    requestEndTimeMillis = 0;

    savedToFile = false;
    displayResultsAsJson = false;

    requestedIp = requestedIp;
    savedFileName = nullptr;
    replyJson = nullptr;
    netReply = nullptr;
}

TabbedResultWidget::~TabbedResultWidget() noexcept {
    // Delete widgets
}