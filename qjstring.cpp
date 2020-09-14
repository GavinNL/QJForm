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
#include <QLine>
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
    //this->setLayout(h);

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
    //this->setLayout(h);

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
    double mm = static_cast<double>( std::numeric_limits<int>::lowest() );
    double MM = static_cast<double>( std::numeric_limits<int>::max() );
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
    //this->setLayout(h);

}

QJArray::~QJArray()
{

}

void QJArray::_rebuild()
{
    auto * L = dynamic_cast<QFormLayout*>(layout());

    while(L->rowCount())
    {
        auto h = L->takeRow( L->rowCount()-1 );
        bool found=false;
        for(auto & x : m_items)
        {
            if( h.fieldItem == x.m_layout)
                found=true;
        }
        if( !found )
        {
            delete h.fieldItem;
        }
    }

    for(auto & x : m_items)
    {
        L->addRow( x.m_layout );
    }

}

void QJArray::setSchema(const QJsonObject &J)
{
    if( J.contains("items"))
    {
        auto p = J.find("items")->toArray();

        int ro=0;
        for(auto i=p.begin(); i!=p.end();i++)
        {
            auto * w = new QJValue(this);
            auto * h = new QHBoxLayout();

            w->setSchema( i->toObject() );

            h->addWidget(w);

            auto * up   = new QToolButton(this); up->setArrowType(Qt::UpArrow);
            auto * down = new QToolButton(this); down->setArrowType(Qt::DownArrow);
            auto * del  = new QToolButton(this); del->setText("✖");

            connect( down, &QAbstractButton::clicked,
                     [w, this](bool)
            {
                size_t ro=0;
                for(auto & x : m_items)
                {
                    if( x.m_widget == w)
                    {
                        if( ro != m_items.size()-1)
                        {
                            std::swap( m_items[ro], m_items[ro+1]);
                            _rebuild();
                            return;
                        }
                    }
                    ro++;
                }
            });

            connect( up, &QAbstractButton::clicked,
                     [w, this](bool)
            {
                size_t ro=0;
                for(auto & x : m_items)
                {
                    if( x.m_widget == w)
                    {
                        if( ro!=0)
                        {
                            std::swap( m_items[ro-1], m_items[ro]);
                            _rebuild();
                            return;
                        }
                    }
                    ro++;
                }
            });

            connect( del, &QAbstractButton::clicked,
                     [w, this](bool)
            {
                int ro=0;
                for(auto & x : m_items)
                {
                    if( x.m_widget == w)
                    {
                        delete x.m_widget;
                        delete x.m_down;
                        delete x.m_up;
                        delete x.m_del;
                        delete x.m_layout;
                        m_items.erase( m_items.begin()+ro);
                        return;
                    }
                    ro++;
                }
            });

            //QPalette pal = del->palette();
            //pal.setColor(QPalette::Base, QColor(Qt::red));
            //del->setAutoFillBackground(true);
            //del->setPalette(pal);
            //del->update();


            h->addWidget( up);
            h->addWidget( down);
            h->addWidget( del);

//            L->addRow(h);

            m_items.push_back({w,h, up, down, del});
            ro++;
        }
    }
    _rebuild();
}

QJsonValue QJArray::getValue() const
{
    QJsonArray O;
    for(auto & x : m_items)
    {
        O.push_back( x.m_widget->getValue() );
    }
    return O;
}



//==============================================================================


QJObject::QJObject(QWidget *parent) :
    QJBase(parent)
{
    QFormLayout * l  = new QFormLayout(this);

    m_propertiesLayout  = new QFormLayout();


    {
        m_oneOf = new QComboBox(this);

        connect(m_oneOf, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [this](int i)
        {
            this->setOneOf( m_oneOfArray[i].toObject() );//->setText( m_Combo->currentText() );
        });

        l->addWidget(m_oneOf);
    }

    m_propertiesLayout->setMargin(3);

    l->addRow(m_propertiesLayout);
}

QJObject::~QJObject()
{

}

void QJObject::setSchema(const QJsonObject &J)
{
    {
        m_oneOf->clear();
        auto oneOfI = J.find("oneOf");
        if( oneOfI != J.end())
        {
            m_oneOfArray = oneOfI->toArray();
            for(auto x : m_oneOfArray)
            {
                QString label = x.toObject().find("title")->toString();
                m_oneOf->addItem(label);
            }
            m_oneOf->setVisible(true);
        }
        else
        {
            m_oneOf->setVisible(false);
        }
    }

    setOneOf(J);
}


void QJObject::setOneOf(const QJsonObject &J)
{
    auto * L = m_propertiesLayout;

    while(m_propertiesLayout->rowCount())
    {
        m_propertiesLayout->removeRow( m_propertiesLayout->rowCount()-1);
    }
    m_properties.clear();

    std::map< QString, QWidget*> props;

    if( J.contains("properties"))
    {
        auto p = J.find("properties")->toObject();

        for(auto i=p.begin(); i!=p.end();i++)
        {
            auto k = i.key();
            auto v = i.value();

            auto * w = new QJValue(this);
            auto vO = v.toObject();

            {
                auto t = vO.find("title");
                if( t!=vO.end()){
                    k = t->toString( k );
                }
            }
            bool visible=true;
            {
                auto t = vO.find("ui:visible");
                if( t!=vO.end()){
                    visible = t->toBool(true);
                }
            }

            {
                auto t = vO.find("description");
                if( t!=vO.end()){
                    w->setToolTip(t->toString());
                }
            }

            w->setSchema(vO);

            if( visible)
            {
                props[k] = w;
            }
            w->setVisible(visible);
            m_properties[ i.key() ] = w;
        }
    }
    if( J.contains("ui:order"))
    {
        auto order = J.find("ui:order")->toArray();
        for(auto x : order)
        {
            L->addRow( x.toString(), props.at(x.toString()));
            props.erase( x.toString() );
        }
    }
    for(auto & x : props)
    {
        L->addRow(x.first,x.second);
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
    //this->setLayout(h);
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














QJForm::QJForm(QWidget *parent) : QWidget(parent)
{
    auto vb     = new QVBoxLayout(this);
    m_ok = new QPushButton("ok");
    m_scrollArea      = new QScrollArea(this);

    vb->addWidget( m_scrollArea );
    vb->addWidget( m_ok );


    connect( m_ok, &QPushButton::clicked,
             [this](bool)
            {
                if( m_jvalue)
                {
                    auto root = m_jvalue->getValue().toObject();
                    emit update(root);
                }
            });

}

void QJForm::setSchema(const QJsonObject &J)
{
    if( m_jvalue)
        delete m_jvalue;
    m_jvalue = new QJValue(this);
    m_jvalue->setSchema(J);
    m_scrollArea->setWidget(m_jvalue);
    m_scrollArea->setWidgetResizable(true);
}

