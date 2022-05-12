/***************************************************************************
 *   Copyright (C) 2011~2011 by CSSlayer                                   *
 *   wengxt@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "common.h"

// Qt
#include <QDebug>
#include <QProcess>
#include <QClipboard>
#include <QApplication>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QDesktopServices>
#include <QFileDialog>
#include <QWebEnginePage>

// Hotot
#include "hototwebpage.h"
#include "hototrequest.h"
#include "mainwindow.h"

HototWebPage::HototWebPage(MainWindow *window, QObject* parent) :
    QWebEnginePage(parent)
{
    m_mainWindow = window;
}

bool HototWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
    Q_UNUSED(type);
    return handleUri(url.toString());
}

bool HototWebPage::handleUri(const QString& originmsg)
{
    QString msg = originmsg;
    if (msg.startsWith("hotot:")) {
        msg = msg.mid(6);
        QString type = msg.section("/", 0, 0);
        QString method = msg.section("/", 1, 1);
        if (type == "system") {
            if (method == "notify") {
                QString notify_type = QUrl::fromPercentEncoding(msg.section("/", 2, 2).toUtf8());
                QString title = QUrl::fromPercentEncoding(msg.section("/", 3, 3).toUtf8());
                QString summary = QUrl::fromPercentEncoding(msg.section("/", 4, 4).toUtf8());
                QString image = QUrl::fromPercentEncoding(msg.section("/", 5, 5).toUtf8());

                m_mainWindow->notification(notify_type, title, summary, image);
            } else if (method == "unread_alert") {
                QString number = QUrl::fromPercentEncoding(msg.section("/", 2, 2).toUtf8());
                m_mainWindow->unreadAlert(number);
            } else if (method == "load_settings") {
                QString settingString = QUrl::fromPercentEncoding(msg.section("/", 2, -1).toUtf8());
                runJavaScript("hotot_qt = " + settingString + ";");

                // unfortunately proxy settings seem to be gone with the blink-based webview :-(
            } else if (method == "sign_in") {
                m_mainWindow->setSignIn(true);
            } else if (method == "sign_out") {
                m_mainWindow->setSignIn(false);
            }
        } else if (type == "action") {
            if (method == "search") {

            } else if (method == "choose_file") {
                QFileDialog dialog;
                dialog.setAcceptMode(QFileDialog::AcceptOpen);
                dialog.setFileMode(QFileDialog::ExistingFile);
                dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg *.gif)"));
                int result = dialog.exec();
                if (result) {
                    QStringList fileNames = dialog.selectedFiles();
                    if (fileNames.size() > 0) {
                        QString callback = msg.section("/", 2, 2);
                        evaluateJavaScript(QString("%1(\"%2\")").arg(callback, QUrl::fromLocalFile(fileNames[0]).toString().replace("file://", "")));
                    }
                }
            } else if (method == "save_avatar") {
            } else if (method == "log") {
                qDebug() << msg;
            } else if (method == "paste_clipboard_text") {
                triggerAction(QWebEnginePage::Paste);
            } else if (method == "set_clipboard_text") {
                QClipboard *clipboard = QApplication::clipboard();
                if (clipboard)
                    clipboard->setText(msg.section("/", 2, -1));
            }
        } else if (type == "request") {
            QString json = QUrl::fromPercentEncoding(msg.section("/", 1, -1).toUtf8());
            evaluateJavaScript(QString("hotot_qt_request_json = %1 ;").arg(json));
            QString request_uuid = evaluateJavaScript(QString("hotot_qt_request_json.uuid")).toString();
            QString request_method = evaluateJavaScript(QString("hotot_qt_request_json.method")).toString();
            QString request_url = evaluateJavaScript(QString("hotot_qt_request_json.url")).toString();
            QMap<QString, QVariant> request_params = evaluateJavaScript(QString("hotot_qt_request_json.params")).toMap();
            QMap<QString, QVariant> request_headers = evaluateJavaScript(QString("hotot_qt_request_json.headers")).toMap();
            QList<QVariant> request_files = evaluateJavaScript(QString("hotot_qt_request_json.files")).toList();

            HototRequest* request = new HototRequest(
                request_uuid,
                request_method,
                request_url,
                request_params,
                request_headers,
                request_files);
            connect(request, SIGNAL(requestFinished(HototRequest*, QByteArray, QString, bool)), this, SLOT(requestFinished(HototRequest*, QByteArray, QString, bool)));
            if (!request->doRequest())
                delete request;
        }
    } else if (msg.startsWith("file://") || msg.startsWith("qrc:")) {
        return true;
    } else if (msg.startsWith("about:")) {
        return false;
    } else if (msg.startsWith("http://stat.hotot.org")) {
        return false;
    } else {
        QDesktopServices::openUrl(msg);
    }
    return false;
}

void HototWebPage::javaScriptAlert(const QUrl& securityOrigin, const QString & msg)
{
    Q_UNUSED(securityOrigin);
    handleUri(msg);
}

void HototWebPage::requestFinished(HototRequest* request, QByteArray result, QString uuid , bool error)
{
    QString strresult = QString::fromUtf8(result);
    if (error) {
        QString scripts = QString("widget.DialogManager.alert('%1', '%2');\n"
                                  "globals.network.error_task_table['%3']('');\n"
                                 ).arg("Ooops, an Error occurred!", strresult, uuid);
        evaluateJavaScript(scripts);
    } else {
        QString scripts;
        if (strresult.startsWith("[") || strresult.startsWith("{"))
            scripts = QString("globals.network.success_task_table['%1'](%2);"
                             ).arg(uuid, strresult);
        else
            scripts = QString("globals.network.success_task_table['%1']('%2');"
                             ).arg(uuid, strresult);
        evaluateJavaScript(scripts);
    }
    request->deleteLater();
}

QVariant HototWebPage::evaluateJavaScript(const QString& js)
{
    QAtomicInt atomint = -1;
    QVariant retval;

    runJavaScript(js, [&atomint, &retval](const QVariant& result) {
        retval = result;
        atomint++;
    });

    while (atomint == -1)
    {
        QApplication::processEvents();
    }

    return retval;
}
