//
// This code was taken from here:
//
// https://stackoverflow.com/a/62247360


#include <QtCore/QEvent>
#include <QtCore/QCoreApplication>
#include <QtCore/QPropertyAnimation> 
#include <QtGui/QPainter> 
#include <QtGui/QMouseEvent> 
#include <QPainterPath>

#include <QJForm/ToggleSwitch.h>

namespace QJForm
{
///<summary>
/// Toggle button has 2 different design. In the first design, if the ball (thumb) radius is 
/// larger than the slide (track) radius, a flat ball slides and colors from the slide according to 
/// the on / off situation. In the second design, if the ball radius is smaller than the slide radius, 
/// the ball moves according to the on / off status inside the slide and includes the check and uncheck marks.
///</summary>
ToggleSwitch::ToggleSwitch(int trackRadius, int thumbRadius, QWidget* parent)
    : QAbstractButton(parent)
{
    setCheckable(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed));
    mTrackRadius = trackRadius;
    mThumbRadius = thumbRadius;
    mAnimation = new QPropertyAnimation(this);
    mAnimation->setTargetObject(this);

    mMargin = 0 > (mThumbRadius - mTrackRadius) ? 0 : (mThumbRadius - mTrackRadius);
    mBaseOffset = mThumbRadius > mTrackRadius ? mThumbRadius : mTrackRadius;
    mEndOffset.insert(true, 4 * mTrackRadius + 2 * mMargin - mBaseOffset); // width - offset
    mEndOffset.insert(false, mBaseOffset);
    mOffset = mBaseOffset;
    QPalette palette = this->palette();

    if (mThumbRadius > mTrackRadius)
    {
        mTrackColor.insert(true, palette.highlight());
        mTrackColor.insert(false, palette.dark());
        mThumbColor.insert(true, palette.highlight());
        mThumbColor.insert(false, palette.light());
        mTextColor.insert(true, palette.highlightedText().color());
        mTextColor.insert(false, palette.dark().color());
        mThumbText.insert(true, "");
        mThumbText.insert(false, "");
        mOpacity = 0.5;
    }
    else
    {
        mTrackColor.insert(true, palette.highlight());
        mTrackColor.insert(false, palette.dark());
        mThumbColor.insert(true, palette.highlightedText());
        mThumbColor.insert(false, palette.light());
        mTextColor.insert(true, palette.highlight().color());
        mTextColor.insert(false, palette.dark().color());
        mThumbText.insert(true, QChar(0x2714)); // check character
        mThumbText.insert(false, QChar(0x2715)); // uncheck character
        mOpacity = 1.0;
    }
}


ToggleSwitch::~ToggleSwitch()
{
    delete mAnimation;
}

QSize ToggleSwitch::sizeHint() const
{
    int w = 4 * mTrackRadius + 2 * mMargin;
    int h = 2 * mTrackRadius + 2 * mMargin;

    return QSize(w, h);
}

void ToggleSwitch::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPainter::RenderHints m_paintFlags = QPainter::RenderHints(QPainter::Antialiasing |
        QPainter::TextAntialiasing);
    p.setRenderHints(m_paintFlags, true);
    p.setPen(Qt::NoPen);
    bool check = isChecked();
    qreal trackOpacity = mOpacity;
    qreal textOpacity = 1.0;
    qreal thumbOpacity = 1.0;
    QBrush trackBrush;
    QBrush thumbBrush;
    QColor textColor;

    if (this->isEnabled())
    {

        trackBrush = mTrackColor[check];
        thumbBrush = mThumbColor[check];
        textColor = mTextColor[check];
    }
    else
    {
        trackOpacity *= 0.8;
        trackBrush = this->palette().shadow();
        thumbBrush = this->palette().mid();
        textColor = this->palette().shadow().color();
    }

    p.setBrush(trackBrush);
    p.setOpacity(trackOpacity);
    p.drawRoundedRect(mMargin, mMargin, width() - 2 * mMargin, height() - 2 * mMargin, mTrackRadius, mTrackRadius);

    p.setBrush(thumbBrush);
    p.setOpacity(thumbOpacity);
    p.drawEllipse(mOffset - mThumbRadius, mBaseOffset - mThumbRadius, 2 * mThumbRadius, 2 * mThumbRadius);

    p.setPen(textColor);
    p.setOpacity(textOpacity);
    QFont font = p.font();
    font.setPixelSize(1.5*mThumbRadius);
    p.setFont(font);


    // Since the antialiasasing provided by the drawText function is incompetent,
    // DrawPath function preferred. But since the drawPath function is not capable of aligment,
    // Pixel offsets calculated to provide aligment.
    QPainterPath textPath;
    qreal pixelOffset = (qreal)mThumbRadius * (1 - 1 / 1.414);
    textPath.addText(mOffset - mThumbRadius + pixelOffset, mBaseOffset + mThumbRadius - pixelOffset, font, mThumbText.value(check));
    p.drawPath(textPath);


    /*p.drawText(QRectF(mOffset - mThumbRadius,
        mBaseOffset - mThumbRadius,
        2 * mThumbRadius,
        2 * mThumbRadius),
        Qt::AlignCenter,
        mThumbText.value(check));*/
}

void ToggleSwitch::resizeEvent(QResizeEvent* e)
{
    QAbstractButton::resizeEvent(e);
    mOffset = mEndOffset.value(isChecked());
}

void ToggleSwitch::mouseReleaseEvent(QMouseEvent  *e)
{
    QAbstractButton::mouseReleaseEvent(e);
    if (e->button() == Qt::LeftButton)
    {
        mAnimation->setDuration(120);
        mAnimation->setPropertyName("mOffset");
        mAnimation->setStartValue(mOffset);
        mAnimation->setEndValue(mEndOffset[isChecked()]);
        mAnimation->start();
    }
}

void ToggleSwitch::enterEvent(QEvent * event)
{
    setCursor(Qt::PointingHandCursor);
    QAbstractButton::enterEvent(event);
}

void ToggleSwitch::setChecked(bool checked)
{
    QAbstractButton::setChecked(checked);
    mOffset = mEndOffset.value(checked);
}

int ToggleSwitch::offset()
{
    return mOffset;
}

void ToggleSwitch::setOffset(int value)
{
    mOffset = value;
    update();
}
}
