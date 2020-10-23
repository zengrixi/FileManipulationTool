#ifndef CHARCONVERSION_H
#define CHARCONVERSION_H

#include <QFile>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QTextStream>

namespace Chardet {

    class PinyinFormat {
    public:
        enum Format { WITH_TONE_MARK = 0, WITH_TONE_NUMBER, WITHOUT_TONE };
    };

    class PinyinResource {
    private:
        PinyinResource() { }
        static QMap<QString, QString> getResource(const QString &resourceName) {
            QMap<QString, QString> map;
            QFile f(resourceName);
            if (!f.open(QIODevice::ReadOnly)) return map;
            QTextStream in(&f);
            in.setCodec("utf-8");
            QString line;
            while ((line = in.readLine()) != "") {
                QStringList tokens = line.trimmed().split("=");
                map.insert(tokens[0], tokens[1]);
            }
            f.close();
            return map;
        }

    public:
        static QMap<QString, QString> getPinyinResource() { return getResource(":/db/pinyin.db"); }
        static QMap<QString, QString> getMutilPinyinResource() { return getResource(":/db/mutil_pinyin.db"); }
        static QMap<QString, QString> getChineseResource() { return getResource(":/db/chinese.db"); }
    };

    class CharConversion : public QObject {
        Q_OBJECT
    public:
        explicit CharConversion(QObject *parent = nullptr);

        static QChar convertASingleTradCharToSimplifiedChar(QChar c); //转换单个繁体为简体字符
        static QChar convertASingleSimplifiedCharToTradChar(QChar c); //转换单个简化字符为繁体
        static QString convertTradCharactersToSimplifiedCharacters(const QString &str); //将传统字符转换为简体字符
        static QString convertSimplifiedCharactersToTradCharacters(const QString &str); //将简体字转换成繁体字
        static bool determineWhetherACharIsATradChineseChar(const QChar c); //判断某个字符是否为繁体字
        static bool determineWhetherACharIsAChineseChar(const QChar c); //判断某个字符是否为汉字
        static bool determineWhetherTheStrContainsChinese(const QString &str); //确定Str是否包含中文

    private:
        static QString CHINESE_REGEX; // = "[\\u4e00-\\u9fa5]";
        static QMap<QString, QString> CHINESE_MAP; // = PinyinResource.getChineseResource();
    };

    typedef QString (*pFunSimplifiedAndTradConversion)(const QString &str); //简化和传统转换

} // namespace Chardet
#endif // CHARCONVERSION_H
