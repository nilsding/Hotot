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
#include <QApplication>
#include <QDesktopServices>
#include <QWebEngineSettings>
#include <QDir>
#include <QDebug>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QNetworkProxy>
#include <QSettings>
#include <QFontDatabase>
#include <QTimer>
#include <QLocale>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QToolTip>
#include <QCursor>
#include <QGraphicsView>
#include <QTimer>
#include <QCloseEvent>

// Hotot
#include "mainwindow.h"
#include "hototwebpage.h"
#include "trayiconinterface.h"
#include "qttraybackend.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_page(0),
    m_webView(new QWebEngineView),
    m_actionMinimizeToTray(new QAction(tr("&Minimize to Tray"), this)),
    m_signIn(false),
    m_firstLoad(true)
{
#ifdef Q_OS_UNIX
    chdir(PREFIX);
#endif
    setWindowTitle(tr("Hotot"));
    setWindowIcon(QIcon::fromTheme("hotot_qt", QIcon("share/hotot/image/ic64_hotot.png")));
    qApp->setWindowIcon(QIcon::fromTheme("hotot_qt", QIcon("share/hotot/image/ic64_hotot.png")));
    this->resize(QSize(640, 480));
    this->setCentralWidget(m_webView);
    this->setMinimumSize(QSize(400, 400));

    m_menu = new QMenu(this);

    m_actionCompose = new QAction(QIcon(), tr("&Compose"), this);
    connect(m_actionCompose, SIGNAL(triggered()), this, SLOT(compose()));
    m_menu->addAction(m_actionCompose);
    m_actionCompose->setVisible(false);

    QSettings settings("hotot-qt", "hotot");
    m_actionMinimizeToTray->setCheckable(true);
    m_actionMinimizeToTray->setChecked(settings.value("minimizeToTray", false).toBool());
    connect(m_actionMinimizeToTray, SIGNAL(toggled(bool)), this, SLOT(toggleMinimizeToTray(bool)));
    m_menu->addAction(m_actionMinimizeToTray);

    m_actionShow = new QAction(QIcon(), tr("Show &MainWindow"), this);
    connect(m_actionShow, SIGNAL(triggered()), this, SLOT(show()));
    m_menu->addAction(m_actionShow);

    m_actionExit = new QAction(QIcon::fromTheme("application-exit"), tr("&Exit"), this);
    m_actionExit->setShortcut(QKeySequence::Quit);
    connect(m_actionExit, SIGNAL(triggered()), this, SLOT(exit()));
    m_menu->addAction(m_actionExit);

    m_tray = new QtTrayBackend(this);

    m_tray->setContextMenu(m_menu);

    addAction(m_actionExit);

    m_page = new HototWebPage(this);

#ifdef Q_OS_UNIX
    QDir dir(QDir::homePath().append("/.config/hotot-qt"));
#else
    QDir dir(QDesktopServices::storageLocation(QDesktopServices::DataLocation).append("/Hotot"));
#endif

    if (!dir.exists())
        dir.mkpath(".");

    m_confDir = dir.absolutePath();

    m_webView->setPage(m_page);
    m_page->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    m_page->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    m_page->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    m_page->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    m_page->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    m_page->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

#ifdef Q_OS_UNIX
    m_webView->load(QUrl("file://" PREFIX "/share/hotot/index.html"));
#else
    QFileInfo f("share/hotot/index.html");
    m_webView->load(QUrl::fromLocalFile(f.absoluteFilePath()));
#endif
    connect(m_webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(m_page, SIGNAL(linkHovered(QString, QString, QString)), this, SLOT(onLinkHovered(QString, QString, QString)));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (isCloseToExit()) {
        exit();
    } else {
        event->ignore();
        hide();
    }
}

void MainWindow::exit()
{
    qApp->exit();
}

bool MainWindow::isCloseToExit() {
    QAtomicInt atomint = -1;
    bool retval = false;

    m_webView->page()->runJavaScript("conf.settings.close_to_exit", [&atomint, &retval](const QVariant& result) {
        if (result.isValid()) {
            retval = result.toBool();
        }
        atomint++;
    });

    while (atomint == -1)
    {
        QApplication::processEvents();
    }

    return retval;
}

bool MainWindow::isStartMinimized() {
    QAtomicInt atomint = -1;
    bool retval = false;

    m_webView->page()->runJavaScript("conf.settings.starts_minimized", [&atomint, &retval](const QVariant& result) {
        if (result.isValid()) {
            retval = result.toBool();
        }
        atomint++;
    });

    while (atomint == -1)
    {
        QApplication::processEvents();
    }

    return retval;
}

bool MainWindow::isAutoSignIn() {
    QAtomicInt atomint = -1;
    bool retval = false;

    m_webView->page()->runJavaScript("conf.settings.sign_in_automatically", [&atomint, &retval](const QVariant& result) {
        if (result.isValid()) {
            retval = result.toBool();
        }
        atomint++;
    });

    while (atomint == -1)
    {
        QApplication::processEvents();
    }

    return retval;
}

MainWindow::~MainWindow()
{
    QSettings settings("hotot-qt", "hotot");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::loadFinished(bool ok)
{
    // disconnect(m_webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    if (ok) {
        QString confString = QString(
            "hotot_qt_variables = {"
            "      'platform': 'Linux'"
            "    , 'wrapper': 'qt'"
            "    , 'avatar_cache_dir': '%3'"
            "    , 'extra_fonts': %4"
            "    , 'extra_exts': %5"
            "    , 'extra_themes': %6"
            "    , 'locale': '%7'"
            "};").arg(m_confDir)
                 .arg(extraFonts())
                 .arg(extraExtensions())
                 .arg(extraThemes())
                 .arg(QLocale::system().name());

        m_webView->page()->runJavaScript(confString);
        m_webView->page()->runJavaScript(
            "overlay_variables(hotot_qt_variables);"
            "globals.load_flags = 1;");

        if (m_firstLoad) {
            m_firstLoad = false;
            QTimer::singleShot(300, this, SLOT(notifyLoadFinished()));
        }
    }
    else {
        show();
    }
}

void MainWindow::notifyLoadFinished()
{
    QSettings settings("hotot-qt", "hotot");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    if (!isStartMinimized() || !isAutoSignIn()) {
        show();
    }
}

void MainWindow::forceActivateWindow()
{
#ifndef Q_WS_MAC
    activateWindow();
#endif
}

void MainWindow::triggerVisible()
{
#ifndef Q_WS_MAC
    if (isVisible()) {
        setVisible(!isVisible());
    }
    else {
        setVisible(!isVisible());
        setWindowState(windowState() & ~Qt::WindowMinimized);
        forceActivateWindow();
    }
#else
    show();
#endif
}

void MainWindow::notification(QString type, QString title, QString message, QString image)
{
    m_tray->showMessage(type, title, message, image);
}

void MainWindow::activate()
{
    if (!isActiveWindow()) {
        if (!isVisible()) {
#ifndef Q_WS_MAC
            setVisible(true);
#else
            show();
#endif
        }
    }
}

void MainWindow::unreadAlert(QString number)
{
    m_tray->unreadAlert(number);
}

void MainWindow::toggleMinimizeToTray(bool checked)
{
    QSettings settings("hotot-qt", "hotot");
    settings.setValue("minimizeToTray", checked);
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        if (m_actionMinimizeToTray->isChecked() && isMinimized()) {
            QTimer::singleShot(0, this, SLOT(hide()));
            event->ignore();
        }
    }
}

void MainWindow::onLinkHovered(const QString & link, const QString & title, const QString & textContent )
{
    if (!link.isEmpty() && !title.isEmpty()) {
        //QToolTip::showText(QCursor::pos(), title);
    }
}

QString MainWindow::extraFonts()
{
    return toJSArray(QFontDatabase::families());
}

QString MainWindow::extraThemes()
{
    QDir dir(QString(m_confDir).append("/theme"));
    if (!dir.exists())
        return toJSArray();

    QStringList dirList = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    QStringList themeList;

    Q_FOREACH(const QString& themedir, dirList) {
        QFileInfo info1(dir.absoluteFilePath(QString(themedir).append("/info.json")));
        QFileInfo info2(dir.absoluteFilePath(QString(themedir).append("/style.css")));
        if (info1.exists() && info1.isFile() && info2.exists() && info2.isFile())
            themeList << QUrl::fromLocalFile(dir.absoluteFilePath(themedir)).toString();
    }
    return toJSArray(themeList);
}

QString MainWindow::extraExtensions()
{
    QDir dir(QString(m_confDir).append("/ext"));
    if (!dir.exists())
        return toJSArray();

    QStringList dirList = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    QStringList extJSList;

    Q_FOREACH(const QString& extdir, dirList) {
        QFileInfo info(dir.absoluteFilePath(QString(extdir).append("/entry.js")));
        if (info.exists() && info.isFile())
            extJSList << QUrl::fromLocalFile(info.absoluteFilePath()).toString();
    }
    return toJSArray(extJSList);
}

QString MainWindow::toJSArray(const QStringList& list)
{
    QString itemString;
    bool first = true;
    Q_FOREACH(const QString& item, list)
    {
        QString s = item;
        s.replace("\\", "\\\\");
        s.replace("'", "\\'");
        if (!first) {
            itemString.append(",");
        }
        itemString.append("'").append(s).append("'");
        first = false;
    }

    return QString("[%1]").arg(itemString);
}

void MainWindow::setSignIn(bool sign)
{
    m_signIn = sign;
    m_actionCompose->setVisible(m_signIn);
}

void MainWindow::compose()
{
    if (m_signIn) {
        m_webView->page()->runJavaScript("ui.StatusBox.open();");
        show();
        forceActivateWindow();
    }
}
