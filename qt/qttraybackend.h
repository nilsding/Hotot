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

// Qt
#include <QSystemTrayIcon>

// Hotot
#include "trayiconinterface.h"

class QtTrayBackend : public QObject,
                      public TrayIconInterface
{
    Q_OBJECT
    Q_INTERFACES(TrayIconInterface)
public:
    QtTrayBackend(MainWindow* parent = 0);
    virtual void setContextMenu(QMenu* menu);
    virtual void showMessage(QString type, QString title, QString message, QString image);
    virtual void unreadAlert(QString number);
protected Q_SLOTS:
    void trayIconClicked(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
private:
    MainWindow* m_mainWindow;
    QSystemTrayIcon* m_trayicon;
};
