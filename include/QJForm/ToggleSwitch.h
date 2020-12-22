//
// This code was taken from here:
//
// https://stackoverflow.com/a/62247360


#ifndef QJFORM_TOGGLEBUTTON_H
#define QJFORM_TOGGLEBUTTON_H

#include <QtWidgets/QAbstractButton>


class QPropertyAnimation;

namespace QJForm
{


class ToggleSwitch :public QAbstractButton
{
    Q_OBJECT
        Q_PROPERTY(int mOffset READ offset WRITE setOffset);
public:
    explicit ToggleSwitch(int trackRadius, int thumbRadius, QWidget* parent = nullptr);
    ~ToggleSwitch();

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent*) override;
    void mouseReleaseEvent(QMouseEvent  *) override;
    void enterEvent(QEvent *event) override;
    void setChecked(bool checked);

    int offset();
    void setOffset(int value);

private:
    qreal mOffset;
    qreal mBaseOffset;
    qreal mMargin;
    qreal mTrackRadius;
    qreal mThumbRadius;
    qreal mOpacity;
    QPropertyAnimation* mAnimation;

    QHash<bool, qreal> mEndOffset;
    QHash<bool, QBrush> mTrackColor;
    QHash<bool, QBrush> mThumbColor;
    QHash<bool, QColor> mTextColor;
    QHash<bool, QString> mThumbText;

};

}


#endif // TOGGLEBUTTON_H
