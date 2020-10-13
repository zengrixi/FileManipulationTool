#include "picturebannerwidget.h"

#include <QTimer>
#include <QPainter>
#include <QBoxLayout>
#include <QMouseEvent>
#include <QPropertyAnimation>

const QColor frontColor = QColor("#DC0000");
const QColor backColor = QColor("#C8C8C8");

PictureBannerArrowWidget::PictureBannerArrowWidget(PictureBannerArrowWidget::ArrowType type, QWidget *parent)
    : QWidget(parent)
    , _bMouseHover(false)
    , _bMousePress(false)
    , _type(type)
{
    setFixedSize(12, 20);           //设置固定大小
}

void PictureBannerArrowWidget::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    _bMouseHover = true;
    update();
}

void PictureBannerArrowWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    _bMouseHover = true;
    update();
}

void PictureBannerArrowWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _bMousePress = true;
        emit clicked();
    }
}

void PictureBannerArrowWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _bMousePress = false;
    }
}

void PictureBannerArrowWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);            //设置渲染提示

    QPen pen;
    pen.setWidth(3);
    pen.setBrush(_bMouseHover ? QColor("#FFFFFF") : QColor("#969696"));

    painter.setPen(pen);

    int margin = 2;
    switch (_type)
    {
        case Right:
        {
            painter.drawLine(QPointF(margin, margin), QPointF(width()-margin, height()/2));
            painter.drawLine(QPointF(margin, height()-margin), QPointF(width()-margin, height()/2));
            break;
        }
        case Left:
        {
            painter.drawLine(QPointF(width()-margin, margin), QPointF(margin, height()/2));
            painter.drawLine(QPointF(width()-margin, height()-margin), QPointF(margin, height()/2));
            break;
        }
    }
}

PictureBannerIndicator::PictureBannerIndicator(QWidget *parent)
    : QWidget(parent)
    , _selected(false)
{

}

void PictureBannerIndicator::select(bool selected)
{
    _selected = selected;
    update();
}

void PictureBannerIndicator::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    emit entered();
}

void PictureBannerIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(_selected ? frontColor : backColor));
    painter.drawRect(rect());
}

PictureBannerPage::PictureBannerPage(QWidget *parent)
    : QLabel(parent)
    , _bActive(false)
{

}

void PictureBannerPage::setActive(bool active)
{
    _bActive = active;
}

void PictureBannerPage::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
}

void PictureBannerPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QLabel::paintEvent(event);

    if (!_bActive)
    {
        QPainter painter(this);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(10,10,10, 100));
        painter.drawRect(rect());
    }
}

PictureBannerView::PictureBannerView(QWidget *parent)
    : QWidget(parent)
    , _size(QSize(400, 500))
    , _leftPage(new PictureBannerPage(this))
    , _centerPage(new PictureBannerPage(this))
    , _rightPage(new PictureBannerPage(this))
{
    _centerPage->setActive(true);           //设为有效

    connect(_leftPage, SIGNAL(clicked()), this, SLOT(slotPageClicked()));
    connect(_centerPage, SIGNAL(clicked()), this, SLOT(slotPageClicked()));
    connect(_rightPage, SIGNAL(clicked()), this, SLOT(slotPageClicked()));

    _leftAnimation = new QPropertyAnimation(_leftPage, "geometry");
    _leftAnimation->setDuration(200);           //设置持续时间
    _leftAnimation->setEasingCurve(QEasingCurve::Linear);           //设置缓和曲线

    _centerAnimation = new QPropertyAnimation(_centerPage, "geometry");
    _centerAnimation->setDuration(200);
    _centerAnimation->setEasingCurve(QEasingCurve::Linear);

    _rightAnimation = new QPropertyAnimation(_rightPage, "geometry");
    _rightAnimation->setDuration(200);
    _rightAnimation->setEasingCurve(QEasingCurve::Linear);

    _centerPage->setFixedSize(_size);
    _leftPage->setMinimumWidth(_size.width()/4);
    _leftPage->setMaximumWidth(_size.width()*2/3);
    _leftPage->setFixedHeight(_size.height()-12);
    _rightPage->setMinimumWidth(_size.width()/4);
    _rightPage->setMaximumWidth(_size.width()*2/3);
    _rightPage->setFixedHeight(_size.height()-12);

    _leftArrow = new PictureBannerArrowWidget(PictureBannerArrowWidget::Left, this);
    _rightArrow = new PictureBannerArrowWidget(PictureBannerArrowWidget::Right, this);
    setArrowHidden(true);           //设置隐藏的箭头
    connect(_leftArrow, SIGNAL(clicked()), this, SLOT(slotArrowClicked()));
    connect(_rightArrow, SIGNAL(clicked()), this, SLOT(slotArrowClicked()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addStretch(1);
    layout->addWidget(_leftPage, 0, Qt::AlignBottom);
    layout->addWidget(_centerPage, 0, Qt::AlignBottom);
    layout->addWidget(_rightPage, 0, Qt::AlignBottom);
    layout->addStretch(1);
    setLayout(layout);
}

PictureBannerView::~PictureBannerView()
{
    delete _leftPage;
    delete _centerPage;
    delete _rightPage;
    delete _leftArrow;
    delete _rightArrow;
    delete _leftAnimation;
    delete _centerAnimation;
    delete _rightAnimation;
}

void PictureBannerView::setLeftPixmap(const QPixmap &pixmap)
{
    setPagePixmap(_leftPage, pixmap);
}

void PictureBannerView::setCenterPixmap(const QPixmap &pixmap)
{
    setPagePixmap(_centerPage, pixmap);
}

void PictureBannerView::setRightPixmap(const QPixmap &pixmap)
{
    setPagePixmap(_rightPage, pixmap);
}

void PictureBannerView::startAnimation(int direction)           //开始动画
{
    QRect leftRect = _leftPage->geometry();
    QRect centerRect = _centerPage->geometry();
    QRect rightRect = _rightPage->geometry();

    QRect tmpRect = QRect(centerRect.x(), centerRect.y()+12,
                          centerRect.width(), centerRect.height()-12);
    if (direction < 0)
    {
        _leftAnimation->setStartValue(rightRect);
        _leftAnimation->setEndValue(leftRect);
        _centerAnimation->setStartValue(leftRect);
        _centerAnimation->setEndValue(centerRect);
        _rightAnimation->setStartValue(tmpRect);
        _rightAnimation->setEndValue(rightRect);

    }
    else if (direction > 0)
    {
        _leftAnimation->setStartValue(tmpRect);
        _leftAnimation->setEndValue(leftRect);
        _centerAnimation->setStartValue(rightRect);
        _centerAnimation->setEndValue(centerRect);
        _rightAnimation->setStartValue(leftRect);
        _rightAnimation->setEndValue(rightRect);
    }

    _rightAnimation->start();
    _centerAnimation->start();
    _leftAnimation->start();
}

void PictureBannerView::slotPageClicked()
{
    PictureBannerPage *page = qobject_cast<PictureBannerPage*>(sender());
    if (page == nullptr)
    {
        return;
    }

    int flag = 0;
    if (page == _leftPage)
    {
        flag = -1;
    }
    else if (page == _rightPage)
    {
        flag = 1;
    }
    emit clicked(flag);
}

void PictureBannerView::slotArrowClicked()
{
    PictureBannerArrowWidget *arrow = qobject_cast<PictureBannerArrowWidget*>(sender());
    if (arrow == nullptr)
    {
        return;
    }

    if (arrow == _leftArrow)
    {
        emit clicked(-1);
    }
    else if (arrow == _rightArrow)
    {
        emit clicked(1);
    }
}

void PictureBannerView::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    setArrowHidden(false);
}

void PictureBannerView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    setArrowHidden(true);
}

void PictureBannerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QWidget::paintEvent(event);
}

void PictureBannerView::setPagePixmap(PictureBannerPage *page, const QPixmap &pixmap)
{
    if (page)
    {
        page->setPixmap(pixmap.scaled(_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}

void PictureBannerView::setArrowHidden(bool hidden)
{
    _leftArrow->setHidden(hidden);
    _rightArrow->setHidden(hidden);

    if (!hidden)
    {
        _leftArrow->move(_leftPage->x()+8, _leftPage->y()+(_leftPage->height()-_leftArrow->height())/2);
        _rightArrow->move(_rightPage->x()+_rightPage->width()-_rightArrow->width()-8,
                           _rightPage->y()+(_rightPage->height()-_rightArrow->height())/2);
        _leftArrow->raise();
        _rightArrow->raise();
    }
}


PictureBannerWidget::PictureBannerWidget(QWidget *parent)
    : QWidget(parent)
{
    _view = nullptr;
    _indLayout = nullptr;
    _indicators.clear();
    _pixmaps.clear();
    _currentIndex = -1;
    _timer = nullptr;
    _interval = 2000;           //间隔= 2000

    init();
}

void PictureBannerWidget::addPage(const QPixmap &pixmap)
{
    _pixmaps.append(pixmap);

    PictureBannerIndicator *ind = new PictureBannerIndicator();
    ind->setFixedSize(18, 3);
    connect(ind, SIGNAL(entered()), this, SLOT(switchIndicator()));
    _indicators.append(ind);
    _indLayout->addWidget(ind);

    if (_currentIndex == -1)
    {
        _currentIndex = 0;
        _indicators.at(_currentIndex)->select(true);
    }
    play(_currentIndex);
}

void PictureBannerWidget::addPages(const QList<QPixmap> &pixmaps)
{
    foreach (QPixmap pixmap, pixmaps)
    {
        addPage(pixmap);
    }
}

void PictureBannerWidget::startAutoPlay()
{
    _timer->start(_interval);
}

void PictureBannerWidget::stopAutoPlay()
{
    _timer->stop();
}

void PictureBannerWidget::setDelayTime(int msec, bool start)
{
    if (msec < 500 || msec > 10000)
    {
        return;
    }

    _interval = msec;
    if (start)
    {
        startAutoPlay();
    }
}

QSize PictureBannerWidget::sizeHint() const
{
    return QSize(600, 180);
}

void PictureBannerWidget::switchIndicator()
{
    PictureBannerIndicator *ind = qobject_cast<PictureBannerIndicator*>(sender());
    if (ind == nullptr)
    {
        return;
    }

    int index = _indicators.indexOf(ind);
    if (index == _currentIndex)
    {
        return;
    }

    if (_currentIndex != -1)
    {
        _indicators.at(_currentIndex)->select(false);
    }

    int flag = 0;
    if (_currentIndex < index)
    {
        flag = 1;
    }
    else if (_currentIndex > index)
    {
        flag = -1;
    }

    _currentIndex = index;
    _indicators.at(_currentIndex)->select(true);

    play(_currentIndex);
    _view->startAnimation(flag);
}

void PictureBannerWidget::switchPage(int flag)
{
    if (_currentIndex == -1)
    {
        return;
    }

    if (flag == 0)
    {
        emit currentClicked(_currentIndex);
        return;
    }

    int count = _indicators.count();
    if (count < 2)
    {
        return;
    }
    _indicators.at(_currentIndex)->select(false);
    _currentIndex += flag;

    if (_currentIndex == -1)
    {
        _currentIndex = count - 1;
    }
    else if (_currentIndex == count)
    {
        _currentIndex = 0;
    }

    _indicators.at(_currentIndex)->select(true);

    play(_currentIndex);
    _view->startAnimation(flag);
}

void PictureBannerWidget::updateRender()
{
    switchPage(1);
}

void PictureBannerWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void PictureBannerWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    emit close();
}

void PictureBannerWidget::init()
{
    _indLayout = new QHBoxLayout();
    _indLayout->setSpacing(5);
    _view = new PictureBannerView();
    connect(_view, SIGNAL(clicked(int)), this, SLOT(switchPage(int)));

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), SLOT(updateRender()));

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);
    bottomLayout->addStretch();
    bottomLayout->addLayout(_indLayout);
    bottomLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(_view);
    layout->addLayout(bottomLayout);
    setLayout(layout);
}

void PictureBannerWidget::play(int index)
{
    if (index < 0)
    {
        return;
    }

    int count = _indicators.count();
    int leftIndex = 0, rightIndex = 0;
    if (count > 1)
    {
        if (index == 0)
        {
            leftIndex = count - 1;
            rightIndex = index + 1;
        }
        else if (index == count -1)
        {
            leftIndex = index - 1;
            rightIndex = 0;
        }
        else
        {
            leftIndex = index - 1;
            rightIndex = index + 1;
        }
    }

    _view->setLeftPixmap(_pixmaps.at(leftIndex));
    _view->setCenterPixmap(_pixmaps.at(index));
    _view->setRightPixmap(_pixmaps.at(rightIndex));
}
