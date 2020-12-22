#ifndef QJFORM_COLLAPSABLEWIDGET_H
#define QJFORM_COLLAPSABLEWIDGET_H

#include <QToolButton>
#include <QVBoxLayout>
#include <QString>

namespace QJForm
{


class CollapsableWidget : public QWidget
{
Q_OBJECT
public:
    CollapsableWidget(QString const & text, QWidget* parent=nullptr);
    CollapsableWidget(QWidget* parent=nullptr);

    /**
     * @brief setText
     * @parama t
     *
     * Sets the text of the bar
     */
    void setText( QString const &t);

    /**
     * @brief text
     * @return
     *
     * Gets the text of the bar
     */
    QString text() const;

    /**
     * @brief widget
     * @return
     *
     * Gets the central widet
     */
    QWidget* widget();

    /**
     * @brief setWidget
     * @param p
     *
     * Sets the central widget
     */
    void setWidget(QWidget* p);

signals:
    void toggled(bool b);

public slots:
    void toggle(bool b);

private:
    QToolButton * m_button = nullptr;
    QVBoxLayout * m_layout = nullptr;
    QWidget     * m_centralWidget = nullptr;
};

}

#endif
