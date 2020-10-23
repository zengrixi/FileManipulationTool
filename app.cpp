#include "app.h"

QString App::AppName = "文件重命名工具";
QString App::AppPath = "./";

QString App::ConfigFile = "config.ini";

void App::readConfig() {
    if (!checkConfig()) { return; }

    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("App");
    set.endGroup();
}

void App::writeConfig() {
    QSettings set(App::ConfigFile, QSettings::IniFormat);

    set.beginGroup("App");
    set.setValue("", "");
    set.endGroup();
}

void App::newConfig() {
    writeConfig();
}

bool App::checkConfig() {
    //如果配置文件大小为0,则以初始值继续运行,并生成配置文件
    QFile file(App::ConfigFile);
    if (0 == file.size()) {
        newConfig();
        return false;
    }

    //如果配置文件不完整,则以初始值继续运行,并生成配置文件
    if (file.open(QFile::ReadOnly)) {
        auto ok = true;
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            QStringList list = line.split("=");

            if (2 == list.count()) {
                if ("" == list.at(1)) {
                    ok = false;
                    break;
                }
            }
        }

        if (!ok) {
            newConfig();
            return false;
        }
    } else {
        newConfig();
        return false;
    }

    return true;
}
