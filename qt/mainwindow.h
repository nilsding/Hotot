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

#pragma once

#include "common.h"

// Qt
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QFontDatabase>

class QWebEngineView;
struct TrayIconInterface;

class HototWebPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void notification(QString type, QString title, QString message, QString image);
    void triggerVisible();
    void activate();
    void unreadAlert(QString number);
    void setSignIn(bool sign);

protected Q_SLOTS:
    void loadFinished(bool ok);
    void notifyLoadFinished();
    void onLinkHovered(const QString & link, const QString & title, const QString & textContent );
    void exit();
    void compose();
    void toggleMinimizeToTray(bool checked);

protected:
    void forceActivateWindow();
    bool isCloseToExit();
    bool isStartMinimized();
    bool isAutoSignIn();
    void closeEvent(QCloseEvent *evnet);
    void changeEvent(QEvent *event);

    QString extraFonts();
    QString extraExtensions();
    QString extraThemes();
    static QString toJSArray(const QStringList &list = QStringList());

private:
    HototWebPage* m_page;
    QWebEngineView* m_webView;
    QMenu* m_menu;
    TrayIconInterface* m_tray;
    QAction* m_actionShow;
    QAction* m_actionExit;
    QAction* m_actionDev;
    QAction* m_actionCompose;
    QAction* m_actionMinimizeToTray;
    QString m_confDir;
    bool m_signIn;
    bool m_firstLoad;
};
