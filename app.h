#ifndef APP_H
#define APP_H

#include <QString>
#include <QtCore>

class App {
public:
    static QString AppName; //应用名称
    static QString AppPath; //应用路径
    static QString ConfigFile;

    static void readConfig();
    static void writeConfig();
    static void newConfig();
    static bool checkConfig();
};

#endif // APP_H
