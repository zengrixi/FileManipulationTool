#include "charconversion.h"

namespace Chardet {

QString CharConversion::CHINESE_REGEX = "[\\u4e00-\\u9fa5]";
QMap<QString, QString> CharConversion::CHINESE_MAP = PinyinResource::getChineseResource();

CharConversion::CharConversion(QObject *parent)
    : QObject(parent)
{

}

QChar CharConversion::convertASingleTradCharToSimplifiedChar(QChar c)
{
    auto simplifiedChinese = CHINESE_MAP.value(c);
    return simplifiedChinese.isEmpty() ? c : simplifiedChinese.at(0);
}

QChar CharConversion::convertASingleSimplifiedCharToTradChar(QChar c)
{
    auto tradChinese = CHINESE_MAP.key(c);
    return tradChinese.isEmpty() ? c : tradChinese.at(0);
}

QString CharConversion::convertTradCharactersToSimplifiedCharacters(const QString &str)
{
    QString tmpStr;
    foreach (auto c, str)
    {
        tmpStr += convertASingleTradCharToSimplifiedChar(c);
    }
    return tmpStr;
}

QString CharConversion::convertSimplifiedCharactersToTradCharacters(const QString &str)
{
    QString tmpStr;
    foreach (auto c, str)
    {
        tmpStr += convertASingleSimplifiedCharToTradChar(c);
    }
    return tmpStr;
}

bool CharConversion::determineWhetherACharIsATradChineseChar(const QChar c)
{
    return CHINESE_MAP.keys().indexOf(c) != -1;
}

bool CharConversion::determineWhetherACharIsAChineseChar(const QChar c)
{
    return L'〇' == c || (c.unicode() >= 0x4e00 && c.unicode() <= 0x9fa5);
}

bool CharConversion::determineWhetherTheStrContainsChinese(const QString &str)
{
    foreach (auto c, str)
    {
        if (determineWhetherACharIsAChineseChar(c))
            return true;
    }
    return false;
}

}
