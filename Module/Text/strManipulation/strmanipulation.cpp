#include "strmanipulation.h"

StrManipulation::StrManipulation(QObject *parent) : QObject(parent) { }

int StrManipulation::srchStrForwardOrBackward(const QString &str, const QString &substring, bool direction) {
    return direction ? str.indexOf(substring) : str.lastIndexOf(substring);
}
