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

// System
#include <stdio.h>

// Hotot
#include "mainwindow.h"

void Usage()
{
    printf("Usage: hotot-qt [options]\n"
           "\t\t-h\tShow this help\n"
          );
}

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN32) && !defined(Q_OS_MAC)
    setlocale(LC_ALL, "");
    bindtextdomain("hotot", LOCALEDIR);
    bind_textdomain_codeset("hotot", "UTF-8");
    textdomain("hotot");
#endif
    QApplication a(argc, argv);

    int opt;
    while ((opt = getopt(argc, argv, "sh")) != -1) {
        switch (opt) {
        case 'h':
            Usage();
            return 0;
        default:
            Usage();
            exit(EXIT_FAILURE);
            break;
        }
    }
    MainWindow w;

    return a.exec();
}
