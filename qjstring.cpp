#include "qjstring.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QJsonArray>
#include <QSlider>
#include <QFileDialog>
#include <QTextEdit>
#include <iostream>

QJBase::QJBase(QWidget *parent) :
    QWidget(parent)
{
}


QJString::QJString(QWidget *parent) :
    QJBase(parent)
{
    QHBoxLayout * h  = new QHBoxLayout(this);
    h->setMargin(0);
    this->setLayout(h);

    m_widget = new QLineEdit(this);
    m_Combo = new QComboBox(this);
    m_fileButton = new QPushButton(this);
    m_dirButton = new QPushButton(this);

    h->addWidget(m_widget,0);
    h->addWidget(m_fileButton,0);
    h->addWidget(m_dirButton,0);
    h->addWidget(m_Combo,0);

    connect(m_fileButton, &QPushButton::clicked,
    [this](bool )
    {
        //auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
        auto fileName = QFileDialog::getOpenFileName(this);
        m_widget->setText(fileName);
        //m_fileButton->setText(fileName);
    });

    connect(m_dirButton, &QPushButton::clicked,
    [this](bool )
    {
        //auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
        auto fileName = QFileDialog::getExistingDirectory(this);
        m_widget->setText(fileName);
        //m_fileButton->setText(fileName);
    });

    connect(m_Combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
    [this](int)
    {
        m_widget->setText( m_Combo->currentText() );
    });
}

void QJString::setSchema(const QJsonObject &J)
{
    QString wid = "";
    {
        auto mI = J.find("default");
        if( mI != J.end() )
        {
            auto def = mI->toString();
            m_widget->setText(def);
        }
    }
    {
        auto mI = J.find("ui:widget");
        if( mI != J.end() )
        {
            wid = mI->toString();
        }
    }

    {
        auto mI = J.find("enum");
        if( mI != J.end() )
        {
            auto e = mI->toArray();

            for(auto en : e)
            {
                if( en.isString())
                {
                    m_Combo->addItem( en.toString() );
                }
            }

            m_fileButton->setVisible(false);
            m_dirButton->setVisible(false);
            m_widget->setVisible(false);
            m_Combo->setVisible(true);
            return;
        }
    }


    if( wid == "file")
    {
        m_fileButton->setVisible(true);
        m_dirButton->setVisible(false);
        m_Combo->setVisible(false);
    }
    else if( wid == "dir")
    {
        m_fileButton->setVisible(false);
        m_dirButton->setVisible(true);
        m_Combo->setVisible(false);
    }
    else
    {
        m_dirButton->setVisible(false);
        m_fileButton->setVisible(false);
        m_Combo->setVisible(false);
    }

}

QJString::~QJString()
{

}


QJsonValue QJString::getValue() const
{
    if( auto * w = dynamic_cast<QLineEdit*>(m_widget))
    {
        return w->text();
    }
    return {};
}

//==============================================================================



QJNumber::QJNumber(QWidget *parent) :
    QJBase(parent)
{
    QHBoxLayout * h  = new QHBoxLayout(this);
    h->setMargin(0);
    this->setLayout(h);

    QDoubleSpinBox   * le = new QDoubleSpinBox(this);
    m_slider = new QSlider(Qt::Horizontal, this);
    m_widget = le;

    h->addWidget(m_slider,10);
    h->addWidget(le, 1);

    connect(m_slider, &QSlider::valueChanged,
                //m_widget,
            [this](int i)
            {
                double t = static_cast<double>(i - m_slider->minimum()) / static_cast<double>( m_slider->maximum() - m_slider->minimum());
                double R = m_widget->minimum() * (1.0-t) + m_widget->maximum() * t;
                m_widget->setValue(R);
            }
            );

    connect(m_widget, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double i)
            {
                double t = static_cast<double>(i - m_widget->minimum()) / static_cast<double>( m_widget->maximum() - m_widget->minimum());
                double R = m_slider->minimum() * (1.0-t) + m_slider->maximum() * t;
                m_slider->blockSignals(true);
                m_slider->setValue( static_cast<int>(R) );
                m_slider->blockSignals(false);
            }
            );

    h->update();

}

QJNumber::~QJNumber()
{

}
void QJNumber::setSchema(const QJsonObject &J)
{
    double mm = std::numeric_limits<double>::lowest();
    double MM = std::numeric_limits<double>::max();
    double def = 0.0;

    {
        auto mI = J.find("minimum");
        if( mI != J.end() )
        {
            mm = mI->toDouble( std::numeric_limits<double>::lowest() );
        }
    }
    {
        auto mI = J.find("maximum");
        if( mI != J.end() )
        {
            MM = mI->toDouble( std::numeric_limits<double>::max() );
        }
    }
    {
        auto mI = J.find("default");
        if( mI != J.end() )
        {
            def = mI->toDouble( 0.0 );
        }
    }

    QString wid = "updown";

    {
        auto mI = J.find("ui:widget");
        if( mI != J.end() )
        {
            wid = mI->toString();
        }
    }

    if( wid == "range")
    {
        m_widget->setButtonSymbols( QAbstractSpinBox::NoButtons );
        m_slider->show();
        m_widget->show();
    }
    else
    {
        m_widget->setButtonSymbols( QAbstractSpinBox::UpDownArrows );
        m_slider->hide();
        m_widget->show();
    }

    m_widget->setRange(mm, MM);
    m_slider->setRange( 0, 0xFFFFFF );
    m_widget->setValue(def);

}

QJsonValue QJNumber::getValue() const
{
    return dynamic_cast<QDoubleSpinBox*>(m_widget)->value();
}


//==============================================================================


QJArray::QJArray(QWidget *parent) :
    QJBase(parent)
{
    QFormLayout * h  = new QFormLayout(this);
    h->setMargin(3);
    this->setLayout(h);

}

QJArray::~QJArray()
{

}

void QJArray::setSchema(const QJsonObject &J)
{
    auto * L = dynamic_cast<QFormLayout*>(layout());

    auto p = J.find("items")->toArray();

    for(auto i=p.begin(); i!=p.end();i++)
    {
        auto * w = new QJValue(this);

        w->setSchema( i->toObject() );
        L->addRow(w);

        m_items.push_back(w);
    }
}

QJsonValue QJArray::getValue() const
{
    QJsonArray O;
    for(auto & x : m_items)
    {
        O.push_back( x->getValue() );
    }
    return O;
}



//==============================================================================


QJObject::QJObject(QWidget *parent) :
    QJBase(parent)
{
    QFormLayout * h  = new QFormLayout(this);
    h->setMargin(3);
    this->setLayout(h);

}

QJObject::~QJObject()
{

}

void QJObject::setSchema(const QJsonObject &J)
{
    auto * L = dynamic_cast<QFormLayout*>(layout());

    auto p = J.find("properties")->toObject();

    for(auto i=p.begin(); i!=p.end();i++)
    {
        auto k = i.key();
        auto v = i.value();

        auto * w = new QJValue(this);
        auto vO = v.toObject();

        auto t = vO.find("title");
        if( t!=vO.end()){
            k = t->toString( k );
        }
        w->setSchema(vO);
        L->addRow( k, w);

        m_properties[ i.key() ] = w;
    }
}

QJsonValue QJObject::getValue() const
{
    QJsonObject O;
    for(auto & x : m_properties)
    {
        O[x.first] = x.second->getValue();
    }
    return O;
}
















QJValue::QJValue(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout * h  = new QHBoxLayout(this);
    h->setMargin(0);
    this->setLayout(h);
}

QJsonValue QJValue::getValue() const
{
    if( auto * w = dynamic_cast<QJNumber*>(m_widget))
    {
        return w->getValue();
    }
    else if( auto * w = dynamic_cast<QJString*>(m_widget))
    {
        return w->getValue();
    }
    else if( auto * w = dynamic_cast<QJObject*>(m_widget))
    {
        return w->getValue();
    }
    else if( auto * w = dynamic_cast<QJArray*>(m_widget))
    {
        return w->getValue();
    }
    return {};
}

void QJValue::setSchema(const QJsonObject &J)
{
    {
        if( m_widget )
            delete m_widget;

        auto type = J.find("type")->toString();

        if( type == "string" )
        {
            auto * w = new QJString(this);
            m_widget = w;
            w->setSchema(J);
            this->layout()->addWidget(m_widget);
        }
        else if( type == "number")
        {
            auto * w = new QJNumber(this);
            m_widget = w;
            w->setSchema(J);
            this->layout()->addWidget(m_widget);
        }
        else if( type == "object")
        {
            auto * w = new QJObject(this);
            m_widget = w;
            w->setSchema(J);
            this->layout()->addWidget(m_widget);
        }
        else if( type == "array")
        {
            auto * w = new QJArray(this);
            m_widget = w;
            w->setSchema(J);
            this->layout()->addWidget(m_widget);
        }

    }
    this->updateGeometry();

}


QJValue::~QJValue()
{
}


