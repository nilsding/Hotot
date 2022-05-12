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
#include <QWebEnginePage>
#include <QByteArray>

class HototRequest;
class MainWindow;
class HototWebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    HototWebPage(MainWindow *mainWindow, QObject* parent = 0);
protected Q_SLOTS:
    void requestFinished(HototRequest* request, QByteArray result, QString uuid , bool error);
protected:
    virtual bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override;
    virtual void javaScriptAlert(const QUrl& securityOrigin, const QString & msg) override;
    bool handleUri(const QString& string);
private:
    MainWindow* m_mainWindow;

    QVariant evaluateJavaScript(const QString& js);
};
