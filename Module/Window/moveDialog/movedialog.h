#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QDialog>

class MoveDialog : public QDialog {
    Q_OBJECT
public:
    explicit MoveDialog(QWidget *parent = nullptr);
    explicit MoveDialog(bool transparent, QWidget *parent = nullptr);

    inline void drawWindowShadow(bool show) { _showShadow = show; }
    inline void blockMoveOption(bool block) { _moveOption = block; }

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    QPoint _pressAt;
    bool _moveOption;
    bool _showShadow;
    bool _leftButtonPress;
};

#endif // MOVEDIALOG_H
