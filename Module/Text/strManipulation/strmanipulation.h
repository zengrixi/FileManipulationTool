#ifndef STRMANIPULATION_H
#define STRMANIPULATION_H

#include <QObject>

class StrManipulation : public QObject
{
    Q_OBJECT
public:
    explicit StrManipulation(QObject *parent = nullptr);
    static int srchStrForwardOrBackward(const QString &str, const QString &substring, bool direction);

signals:

public slots:
};

#endif // STRMANIPULATION_H
