#include <QJForm/CollapsableWidget.h>

QJForm::CollapsableWidget::CollapsableWidget(const QString &text, QWidget *parent) : CollapsableWidget(parent)
{
    setText(text);
}

QJForm::CollapsableWidget::CollapsableWidget(QWidget *parent) : QWidget(parent)
{
    m_layout = new QVBoxLayout();
    m_button = new QToolButton();
    m_button->setCheckable(true);
    m_button->setChecked(true);
    m_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_button->setArrowType(Qt::ArrowType::DownArrow);
    m_button->setStyleSheet( QString(
                                 "QToolButton {"
                                 "background-color: rgb(85, 85, 85);"
                                 "border-style: outset;"
                                 "border-width: 0px;   "
                                 "border-color: beige; "
                                 "}"
                                 "QToolButton::pressed {"
                                 "background-color: rgb(85, 85, 85);"
                                 "border-width: 0px;   "
                                 "}"
                                 "QToolButton::checked {"
                                 "background-color: rgb(85, 85, 85);"
                                 "border-width: 0px;   "
                                 "}"
                                 ) );

    m_button->setLayout( new QHBoxLayout() );
    m_button->setMinimumHeight(20);
    m_layout->setMargin(  0);

    m_layout->setSpacing( 0 );

    m_layout->addWidget(m_button, 0);
    this->setLayout( m_layout );

    m_button->connect( m_button, &QToolButton::toggled, [this](bool togg)
    {
        m_button->setArrowType(togg ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
        if(m_centralWidget)
        {
            m_centralWidget->setVisible(togg);
        }
        emit toggled(togg);
    });
}

void QJForm::CollapsableWidget::setText(const QString &t)
{
    m_button->setText(t);
}

QString QJForm::CollapsableWidget::text() const
{
    return m_button->text();
}

QWidget *QJForm::CollapsableWidget::widget()
{
    return m_centralWidget;
}

void QJForm::CollapsableWidget::setWidget(QWidget *p)
{
    if( m_centralWidget)
        m_centralWidget->deleteLater();

    m_centralWidget = p;
    m_layout->addWidget(p, 1);
}

void QJForm::CollapsableWidget::toggle(bool t)
{
    m_button->setChecked(t);
}
