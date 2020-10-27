// Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
//
// This file is part of Flameshot.
//
//     Flameshot is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     Flameshot is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with Flameshot.  If not, see <http://www.gnu.org/licenses/>.

#include "pathinfo.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>

const QString PathInfo::whiteIconPath()
{
    return QString::fromUtf8(":/img/material/white/");
}

const QString PathInfo::blackIconPath()
{
    return QString::fromUtf8(":/img/material/black/");
}

QStringList PathInfo::translationsPaths()
{
    QString binaryPath =
      QFileInfo(qApp->applicationDirPath()).absoluteFilePath();
    QString trPath = QDir::toNativeSeparators(binaryPath + "/translations");
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    return QStringList()
           << QString::fromUtf8(APP_PREFIX) + "/share/flameshot/translations"
           << trPath << QString::fromUtf8("/usr/share/flameshot/translations")
           << QString::fromUtf8("/usr/local/share/flameshot/translations");
#elif defined(Q_OS_WIN)
    return QStringList() << trPath;
#endif
}
