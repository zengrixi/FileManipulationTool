#ifndef PICTUREBANNERWIDGET_H
#define PICTUREBANNERWIDGET_H

#include <QLabel>

class QHBoxLayout;
class QPropertyAnimation;

class PictureBannerArrowWidget : public QWidget         //图片横幅箭头小部件
{
    Q_OBJECT
public:
    enum ArrowType
    {
        Left,
        Right
    };
    explicit PictureBannerArrowWidget(ArrowType type, QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    bool _bMouseHover;
    bool _bMousePress;
    ArrowType _type;
};


class PictureBannerIndicator : public QWidget           //图片横幅指示器
{
    Q_OBJECT
public:
    explicit PictureBannerIndicator(QWidget *parent = nullptr);
    void select(bool selected);

signals:
    void entered();

protected:
    virtual void enterEvent(QEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    bool _selected;
};

class PictureBannerPage : public QLabel         //图片横幅页面
{
    Q_OBJECT
public:
    explicit PictureBannerPage(QWidget *parent = nullptr);
    void setActive(bool active);

signals:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    bool _bActive;
};


class PictureBannerView : public QWidget            //图片横幅视图
{
    Q_OBJECT
public:
    explicit PictureBannerView(QWidget *parent = nullptr);
    virtual ~PictureBannerView() override;

    void setLeftPixmap(const QPixmap &pixmap);
    void setCenterPixmap(const QPixmap &pixmap);
    void setRightPixmap(const QPixmap &pixmap);

    void startAnimation(int direction);

signals:
    void clicked(int flag);

private slots:
    void slotPageClicked();
    void slotArrowClicked();

protected:
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

    void setPagePixmap(PictureBannerPage *page, const QPixmap &pixmap);
    void setArrowHidden(bool hidden);

    QSize _size;
    PictureBannerPage *_leftPage;
    PictureBannerPage *_centerPage;
    PictureBannerPage *_rightPage;
    PictureBannerArrowWidget *_leftArrow;
    PictureBannerArrowWidget *_rightArrow;
    QPropertyAnimation *_leftAnimation;
    QPropertyAnimation *_centerAnimation;
    QPropertyAnimation *_rightAnimation;
};


class PictureBannerWidget : public QWidget          //图片横幅小工具
{
    Q_OBJECT
public:
    explicit PictureBannerWidget(QWidget *parent = nullptr);

    void addPage(const QPixmap &pixmap);
    void addPages(const QList<QPixmap> &pixmaps);

    void startAutoPlay();
    void stopAutoPlay();

    void setDelayTime(int msec, bool start = false);

    virtual QSize sizeHint() const override;

signals:
    void currentClicked(int index);
    void close();

public slots:
    void switchIndicator();
    void switchPage(int flag);
    void updateRender();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

    void init();
    void play(int index);

    PictureBannerView *_view;
    QHBoxLayout *_indLayout;
    QList<QPixmap> _pixmaps;
    QList<PictureBannerIndicator*> _indicators;
    int _currentIndex;
    int _interval;
    QTimer *_timer;

};

#endif // PICTUREBANNERWIDGET_H
