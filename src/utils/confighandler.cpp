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

#include "confighandler.h"
#include "src/tools/capturetool.h"
#include "src/tools/storage/storagemanager.h"
#include "src/utils/configshortcuts.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QKeySequence>
#include <algorithm>

ConfigHandler::ConfigHandler()
{
    m_settings.setDefaultFormat(QSettings::IniFormat);
}

QVector<CaptureToolButton::ButtonType> ConfigHandler::getButtons()
{
    QVector<CaptureToolButton::ButtonType> buttons;
    if (m_settings.contains(QString::fromUtf8("buttons"))) {
        // TODO: remove toList in v1.0
        QVector<int> buttonsInt = m_settings.value(QString::fromUtf8("buttons"))
                                    .value<QList<int>>()
                                    .toVector();
        bool modified = normalizeButtons(buttonsInt);
        if (modified) {
            m_settings.setValue(QString::fromUtf8("buttons"),
                                QVariant::fromValue(buttonsInt.toList()));
        }
        buttons = fromIntToButton(buttonsInt);
    } else {
        // Default tools
        buttons << CaptureToolButton::TYPE_PENCIL
                << CaptureToolButton::TYPE_DRAWER
                << CaptureToolButton::TYPE_ARROW
                << CaptureToolButton::TYPE_SELECTION
                << CaptureToolButton::TYPE_RECTANGLE
                << CaptureToolButton::TYPE_CIRCLE
                << CaptureToolButton::TYPE_MARKER
                << CaptureToolButton::TYPE_PIXELATE
                << CaptureToolButton::TYPE_SELECTIONINDICATOR
                << CaptureToolButton::TYPE_MOVESELECTION
                << CaptureToolButton::TYPE_UNDO << CaptureToolButton::TYPE_REDO
                << CaptureToolButton::TYPE_COPY << CaptureToolButton::TYPE_SAVE
                << CaptureToolButton::TYPE_EXIT
                << CaptureToolButton::TYPE_IMAGEUPLOADER
                << CaptureToolButton::TYPE_OPEN_APP
                << CaptureToolButton::TYPE_PIN << CaptureToolButton::TYPE_TEXT
                << CaptureToolButton::TYPE_CIRCLECOUNT;
    }

    using bt = CaptureToolButton::ButtonType;
    std::sort(buttons.begin(), buttons.end(), [](bt a, bt b) {
        return CaptureToolButton::getPriorityByButton(a) <
               CaptureToolButton::getPriorityByButton(b);
    });
    return buttons;
}

void ConfigHandler::setButtons(
  const QVector<CaptureToolButton::ButtonType>& buttons)
{
    QVector<int> l = fromButtonToInt(buttons);
    normalizeButtons(l);
    // TODO: remove toList in v1.0
    m_settings.setValue(QString::fromUtf8("buttons"),
                        QVariant::fromValue(l.toList()));
}

QVector<QColor> ConfigHandler::getUserColors()
{
    QVector<QColor> colors;
    const QVector<QColor>& defaultColors = {
        Qt::white,     Qt::red,      Qt::green,       Qt::blue,
        Qt::black,     Qt::darkRed,  Qt::darkGreen,   Qt::darkBlue,
        Qt::darkGray,  Qt::cyan,     Qt::magenta,     Qt::yellow,
        Qt::lightGray, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow,
        QColor()
    };

    if (m_settings.contains(QString::fromUtf8("userColors"))) {
        for (const QString& hex :
             m_settings.value(QString::fromUtf8("userColors")).toStringList()) {
            if (QColor::isValidColor(hex)) {
                colors.append(QColor(hex));
            }
        }

        if (colors.isEmpty()) {
            colors = defaultColors;
        }
    } else {
        colors = defaultColors;
    }

    return colors;
}

void ConfigHandler::setUserColors(const QVector<QColor>& l)
{
    QStringList hexColors;

    for (const QColor& color : l) {
        hexColors.append(color.name());
    }

    m_settings.setValue(QString::fromUtf8("userColors"),
                        QVariant::fromValue(hexColors));
}

QString ConfigHandler::savePath()
{
    return m_settings.value(QString::fromUtf8("savePath")).toString();
}

void ConfigHandler::setSavePath(const QString& savePath)
{
    m_settings.setValue(QString::fromUtf8("savePath"), savePath);
}

bool ConfigHandler::savePathFixed()
{
    if (!m_settings.contains(QString::fromUtf8("savePathFixed"))) {
        m_settings.setValue(QString::fromUtf8("savePathFixed"), false);
    }
    return m_settings.value(QString::fromUtf8("savePathFixed")).toBool();
}

void ConfigHandler::setSavePathFixed(bool savePathFixed)
{
    m_settings.setValue(QString::fromUtf8("savePathFixed"), savePathFixed);
}

QColor ConfigHandler::uiMainColorValue()
{
    QColor res = QColor(116, 0, 150);

    if (m_settings.contains(QString::fromUtf8("uiColor"))) {
        QString hex = m_settings.value(QString::fromUtf8("uiColor")).toString();

        if (QColor::isValidColor(hex)) {
            res = QColor(hex);
        }
    }
    return res;
}

void ConfigHandler::setUIMainColor(const QColor& c)
{
    m_settings.setValue(QString::fromUtf8("uiColor"), c.name());
}

QColor ConfigHandler::uiContrastColorValue()
{
    QColor res = QColor(39, 0, 50);

    if (m_settings.contains(QString::fromUtf8("contrastUiColor"))) {
        QString hex =
          m_settings.value(QString::fromUtf8("contrastUiColor")).toString();

        if (QColor::isValidColor(hex)) {
            res = QColor(hex);
        }
    }

    return res;
}

void ConfigHandler::setUIContrastColor(const QColor& c)
{
    m_settings.setValue(QString::fromUtf8("contrastUiColor"), c.name());
}

QColor ConfigHandler::drawColorValue()
{
    QColor res(Qt::red);

    if (m_settings.contains(QString::fromUtf8("drawColor"))) {
        QString hex =
          m_settings.value(QString::fromUtf8("drawColor")).toString();

        if (QColor::isValidColor(hex)) {
            res = QColor(hex);
        }
    }

    return res;
}

void ConfigHandler::setDrawColor(const QColor& c)
{
    m_settings.setValue(QString::fromUtf8("drawColor"), c.name());
}

bool ConfigHandler::showHelpValue()
{
    bool res = true;
    if (m_settings.contains(QString::fromUtf8("showHelp"))) {
        res = m_settings.value(QString::fromUtf8("showHelp")).toBool();
    }
    return res;
}

void ConfigHandler::setShowHelp(const bool showHelp)
{
    m_settings.setValue(QString::fromUtf8("showHelp"), showHelp);
}

bool ConfigHandler::showSidePanelButtonValue()
{
    return m_settings.value(QString::fromUtf8("showSidePanelButton"), true)
      .toBool();
}

void ConfigHandler::setShowSidePanelButton(const bool showSidePanelButton)
{
    m_settings.setValue(QString::fromUtf8("showSidePanelButton"),
                        showSidePanelButton);
}

bool ConfigHandler::desktopNotificationValue()
{
    bool res = true;
    if (m_settings.contains(QString::fromUtf8("showDesktopNotification"))) {
        res = m_settings.value(QString::fromUtf8("showDesktopNotification"))
                .toBool();
    }
    return res;
}

void ConfigHandler::setDesktopNotification(const bool showDesktopNotification)
{
    m_settings.setValue(QString::fromUtf8("showDesktopNotification"),
                        showDesktopNotification);
}

QString ConfigHandler::filenamePatternValue()
{
    return m_settings.value(QString::fromUtf8("filenamePattern")).toString();
}

void ConfigHandler::setFilenamePattern(const QString& pattern)
{
    return m_settings.setValue(QString::fromUtf8("filenamePattern"), pattern);
}

bool ConfigHandler::disabledTrayIconValue()
{
    bool res = false;
    if (m_settings.contains(QString::fromUtf8("disabledTrayIcon"))) {
        res = m_settings.value(QString::fromUtf8("disabledTrayIcon")).toBool();
    }
    return res;
}

void ConfigHandler::setDisabledTrayIcon(const bool disabledTrayIcon)
{
    m_settings.setValue(QString::fromUtf8("disabledTrayIcon"),
                        disabledTrayIcon);
}

int ConfigHandler::drawThicknessValue()
{
    int res = 0;
    if (m_settings.contains(QString::fromUtf8("drawThickness"))) {
        res = m_settings.value(QString::fromUtf8("drawThickness")).toInt();
    }
    return res;
}

void ConfigHandler::setdrawThickness(const int thickness)
{
    m_settings.setValue(QString::fromUtf8("drawThickness"), thickness);
}

bool ConfigHandler::keepOpenAppLauncherValue()
{
    return m_settings.value(QString::fromUtf8("keepOpenAppLauncher")).toBool();
}

void ConfigHandler::setKeepOpenAppLauncher(const bool keepOpen)
{
    m_settings.setValue(QString::fromUtf8("keepOpenAppLauncher"), keepOpen);
}

bool ConfigHandler::startupLaunchValue()
{
    bool res = true;
    if (m_settings.contains(QString::fromUtf8("startupLaunch"))) {
        res = m_settings.value(QString::fromUtf8("startupLaunch")).toBool();
    }
    if (res != verifyLaunchFile()) {
        setStartupLaunch(res);
    }
    return res;
}

bool ConfigHandler::verifyLaunchFile()
{
    bool res = false;

#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    QString path = QDir::homePath() + "/.config/autostart/Flameshot.desktop";
    res = QFile(path).exists();
#elif defined(Q_OS_WIN)
    QSettings bootUpSettings(
      "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
      QSettings::NativeFormat);
    res = bootUpSettings.value("Flameshot").toString() ==
          QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
#endif
    return res;
}

void ConfigHandler::setStartupLaunch(const bool start)
{
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    QString path = QDir::homePath() + "/.config/autostart/";
    QDir autostartDir(path);
    if (!autostartDir.exists()) {
        autostartDir.mkpath(".");
    }

    QFile file(path + "Flameshot.desktop");
    if (start) {
        if (file.open(QIODevice::WriteOnly)) {
            QByteArray data("[Desktop Entry]\nName=flameshot\nIcon=flameshot"
                            "\nExec=flameshot\nTerminal=false\nType=Application"
                            "\nX-GNOME-Autostart-enabled=true\n");
            file.write(data);
        }
    } else {
        file.remove();
    }
#elif defined(Q_OS_WIN)
    QSettings bootUpSettings(
      "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
      QSettings::NativeFormat);
    // set workdir for flameshot on startup
    QSettings bootUpPath(
      "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App "
      "Paths",
      QSettings::NativeFormat);
    if (start) {
        QString app_path =
          QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        bootUpSettings.setValue("Flameshot", app_path);

        // set application workdir
        bootUpPath.beginGroup("flameshot.exe");
        bootUpPath.setValue("Path", QCoreApplication::applicationDirPath());
        bootUpPath.endGroup();

    } else {
        bootUpSettings.remove("Flameshot");

        // remove application workdir
        bootUpPath.beginGroup("flameshot.exe");
        bootUpPath.remove("");
        bootUpPath.endGroup();
    }
#endif
    m_settings.setValue(QString::fromUtf8("startupLaunch"), start);
}

bool ConfigHandler::showStartupLaunchMessage()
{
    if (!m_settings.contains(QString::fromUtf8("showStartupLaunchMessage"))) {
        m_settings.setValue(QString::fromUtf8("showStartupLaunchMessage"),
                            true);
    }
    return m_settings.value(QString::fromUtf8("showStartupLaunchMessage"))
      .toBool();
}

void ConfigHandler::setShowStartupLaunchMessage(
  const bool showStartupLaunchMessage)
{
    m_settings.setValue(QString::fromUtf8("showStartupLaunchMessage"),
                        showStartupLaunchMessage);
}

int ConfigHandler::contrastOpacityValue()
{
    int opacity = 190;
    if (m_settings.contains(QString::fromUtf8("contrastOpacity"))) {
        opacity =
          m_settings.value(QString::fromUtf8("contrastOpacity")).toInt();
        opacity = qBound(0, opacity, 255);
    }
    return opacity;
}

void ConfigHandler::setContrastOpacity(const int transparency)
{
    m_settings.setValue(QString::fromUtf8("contrastOpacity"), transparency);
}

bool ConfigHandler::closeAfterScreenshotValue()
{
    return m_settings.value(QString::fromUtf8("closeAfterScreenshot")).toBool();
}

void ConfigHandler::setCloseAfterScreenshot(const bool close)
{
    m_settings.setValue(QString::fromUtf8("closeAfterScreenshot"), close);
}

bool ConfigHandler::copyAndCloseAfterUploadEnabled()
{
    bool res = true;
    if (m_settings.contains(QString::fromUtf8("copyAndCloseAfterUpload"))) {
        res = m_settings.value(QString::fromUtf8("copyAndCloseAfterUpload"))
                .toBool();
    }
    return res;
}

void ConfigHandler::setCopyAndCloseAfterUploadEnabled(const bool value)
{
    m_settings.setValue(QString::fromUtf8("copyAndCloseAfterUpload"), value);
}
bool ConfigHandler::saveAfterCopyValue()
{
    return m_settings.value(QString::fromUtf8("saveAfterCopy")).toBool();
}

void ConfigHandler::setSaveAfterCopy(const bool save)
{
    m_settings.setValue(QString::fromUtf8("saveAfterCopy"), save);
}

bool ConfigHandler::copyPathAfterSaveEnabled()
{
    bool res = false;
    if (m_settings.contains(QString::fromUtf8("copyPathAfterSave"))) {
        res = m_settings.value(QString::fromUtf8("copyPathAfterSave")).toBool();
    }
    return res;
}

void ConfigHandler::setCopyPathAfterSaveEnabled(const bool value)
{
    m_settings.setValue(QString::fromUtf8("copyPathAfterSave"), value);
}

void ConfigHandler::setUploadStorage(const QString& uploadStorage)
{
    StorageManager storageManager;
    if (storageManager.storageLocked()) {
        m_settings.setValue(QString::fromUtf8("uploadStorage"),
                            storageManager.storageDefault());
    } else {
        m_settings.setValue(QString::fromUtf8("uploadStorage"), uploadStorage);
    }
}

const QString& ConfigHandler::uploadStorage()
{
    StorageManager storageManager;
    // check for storage lock
    if (storageManager.storageLocked()) {
        setUploadStorage(storageManager.storageDefault());
    }

    // get storage
    m_strRes = m_settings.value(QString::fromUtf8("uploadStorage")).toString();
    if (m_strRes.isEmpty()) {
        StorageManager storageManager;
        m_strRes = storageManager.storageDefault();
        setUploadStorage(m_strRes);
    }
    return m_strRes;
}

QString ConfigHandler::saveAfterCopyPathValue()
{
    return m_settings.value(QString::fromUtf8("saveAfterCopyPath")).toString();
}

void ConfigHandler::setSaveAfterCopyPath(const QString& path)
{
    m_settings.setValue(QString::fromUtf8("saveAfterCopyPath"), path);
}

void ConfigHandler::setDefaults()
{
    m_settings.clear();
}

void ConfigHandler::setAllTheButtons()
{
    QVector<int> buttons;
    auto listTypes = CaptureToolButton::getIterableButtonTypes();
    for (const CaptureToolButton::ButtonType t : listTypes) {
        buttons << static_cast<int>(t);
    }
    // TODO: remove toList in v1.0
    m_settings.setValue(QString::fromUtf8("buttons"),
                        QVariant::fromValue(buttons.toList()));
}

QString ConfigHandler::configFilePath() const
{
    return m_settings.fileName();
}

bool ConfigHandler::normalizeButtons(QVector<int>& buttons)
{
    auto listTypes = CaptureToolButton::getIterableButtonTypes();
    QVector<int> listTypesInt;
    for (auto i : listTypes)
        listTypesInt << static_cast<int>(i);

    bool hasChanged = false;
    for (int i = 0; i < buttons.size(); i++) {
        if (!listTypesInt.contains(buttons.at(i))) {
            buttons.remove(i);
            hasChanged = true;
        }
    }
    return hasChanged;
}

QVector<CaptureToolButton::ButtonType> ConfigHandler::fromIntToButton(
  const QVector<int>& l)
{
    QVector<CaptureToolButton::ButtonType> buttons;
    for (auto const i : l)
        buttons << static_cast<CaptureToolButton::ButtonType>(i);
    return buttons;
}

QVector<int> ConfigHandler::fromButtonToInt(
  const QVector<CaptureToolButton::ButtonType>& l)
{
    QVector<int> buttons;
    for (auto const i : l)
        buttons << static_cast<int>(i);
    return buttons;
}

QVector<QStringList> ConfigHandler::shortcuts()
{
    ConfigShortcuts configShortcuts;
    m_shortcuts = configShortcuts.captureShortcutsDefault(getButtons());
    return m_shortcuts;
}

void ConfigHandler::setShortcutsDefault()
{
    ConfigShortcuts configShortcuts;
    for (auto shortcutItem : shortcuts()) {
        QString shortcutName = shortcutItem.at(0);
        QString shortcutDescription = shortcutItem.at(1);
        QString shortcutValueDefault = shortcutItem.at(2);

        QString shortcutValue = shortcut(shortcutName);

        QKeySequence ks = QKeySequence();
        if (shortcutValue.isNull()) {
            ks = QKeySequence(shortcutValueDefault);
            if (!setShortcut(shortcutName, ks.toString())) {
                shortcutValue = shortcutValueDefault;
            }
        }

        m_shortcuts << (QStringList() << shortcutName << shortcutDescription
                                      << shortcutValue);
    }
}

bool ConfigHandler::setShortcut(const QString& shortcutName,
                                const QString& shortutValue)
{
    bool error = false;
    m_settings.beginGroup("Shortcuts");

    QVector<QKeySequence> reservedShortcuts;
    reservedShortcuts << QKeySequence(Qt::Key_Backspace)
                      << QKeySequence(Qt::Key_Escape);
    if (shortutValue.isEmpty()) {
        m_settings.setValue(shortcutName, "");
    } else if (reservedShortcuts.contains(QKeySequence(shortutValue))) {
        // do not allow to set reserved shortcuts
        error = true;
    } else {
        // Make no difference for Return and Enter keys
        QString shortcutItem = shortutValue;
        if (shortcutItem == "Enter") {
            shortcutItem = QKeySequence(Qt::Key_Return).toString();
        }

        // do not allow to set overlapped shortcuts
        foreach (auto currentShortcutName, m_settings.allKeys()) {
            if (m_settings.value(currentShortcutName) == shortcutItem) {
                m_settings.setValue(shortcutName, "");
                error = true;
                break;
            }
        }
        if (!error) {
            m_settings.setValue(shortcutName, shortcutItem);
        }
    }
    m_settings.endGroup();
    return !error;
}

const QString& ConfigHandler::shortcut(const QString& shortcutName)
{
    m_settings.beginGroup("Shortcuts");
    m_strRes = m_settings.value(shortcutName).toString();
    m_settings.endGroup();
    return m_strRes;
}
