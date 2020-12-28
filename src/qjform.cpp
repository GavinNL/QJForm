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
#include <QColorDialog>
#include <QLine>
#include <QTabWidget>
#include <iostream>
#include <QJsonDocument>
#include <QListWidget>

#include <QJForm/qjform.h>
#include <QJForm/ToggleSwitch.h>

namespace QJForm
{

static QJsonObject getRef(QJsonObject const & J, std::string ref)
{
    {
        auto i = ref.begin();
        auto j = std::find( i, ref.end(), '/');

        QString key = std::string(i,j).c_str();

        if( j==ref.end())
        {
            if( J.contains(key) )
            {
                return J.find(key)->toObject();
            }
            return {};
        }

        auto it = J.find(key);
        if( it != J.end())
        {
            return getRef( it->toObject(), std::string(j+1, ref.end()));
        }
    }
    return {};
}


QJBase::QJBase(QWidget *parent, QJForm *parentForm) :
    QWidget(parent), m_parentForm(parentForm)
{
}


QJString::QJString(QWidget *parent, QJForm *parentForm) :
    QJBase(parent, parentForm)
{
    QHBoxLayout * h  = new QHBoxLayout(this);
    h->setMargin(0);
    //this->setLayout(h);

    m_widget = new QLineEdit(this);
    m_Combo = new QComboBox(this);
    m_fileButton = new QPushButton(this);
    m_dirButton = new QPushButton(this);
    m_colorButton = new QPushButton(this);
    m_dateEdit = new QDateEdit(this);

    h->addWidget(m_widget,0);
    h->addWidget(m_fileButton,0);
    h->addWidget(m_dirButton,0);
    h->addWidget(m_colorButton,0);
    h->addWidget(m_Combo,0);
    h->addWidget(m_dateEdit,0);

    connect(m_widget, &QLineEdit::textChanged,
    [this]()
    {
        emit getParentForm()->changed();
    });

    connect(m_dateEdit, &QDateEdit::dateChanged,
    [this](const QDate &date )
    {
        //auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
        m_widget->setText( date.toString(Qt::ISODate));
        //m_fileButton->setText(fileName);
    });

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

    connect(m_colorButton, &QPushButton::clicked,
    [this](bool )
    {
        //auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
       // auto fileName = QFileDialog::getExistingDirectory(this);
        auto c = QColorDialog::getColor(Qt::white,this,"Choose a Color", QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
        int r,g,b,a;
        c.getRgb(&r,&g,&b,&a);
        char txt[25];
        std::sprintf(txt, "#%02x%02x%02x%02x",r,g,b,a);
        m_widget->setText(txt);

    });

    connect(m_Combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
    [this](int)
    {
        m_widget->setText( m_Combo->currentText() );
    });
}

void QJString::setSchema(const QJsonObject &JJ)
{
    QJsonObject J = getParentForm()->dereference(JJ);

    QString wid = "";

    QString defaultValue;
    {
        auto mI = J.find("default");
        if( mI != J.end() )
        {
            defaultValue = mI->toString();
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
            m_colorButton->setVisible(false);
            m_dateEdit->setVisible(false);
            m_Combo->setVisible(true);
            setValue(defaultValue);
            return;
        }
    }



    //m_widget->setText(defaultValue);
    if( wid == "file")
    {
        m_fileButton->setVisible(true);
        m_dirButton->setVisible(false);
        m_Combo->setVisible(false);
        m_colorButton->setVisible(false);
        m_dateEdit->setVisible(false);
    }
    else if( wid == "date")
    {
        m_dirButton->setVisible(false);
        m_fileButton->setVisible(false);
        m_Combo->setVisible(false);
        m_colorButton->setVisible(false);
        m_widget->setVisible(false);
        m_dateEdit->setVisible(true);
    }
    else if( wid == "dir")
    {
        m_fileButton->setVisible(false);
        m_dirButton->setVisible(true);
        m_Combo->setVisible(false);
        m_colorButton->setVisible(false);
        m_dateEdit->setVisible(false);
    }
    else if( wid=="color" || wid=="colour")
    {
        m_dirButton->setVisible(false);
        m_fileButton->setVisible(false);
        m_Combo->setVisible(false);
        m_colorButton->setVisible(true);
        m_dateEdit->setVisible(false);
    }
    else
    {
        m_dirButton->setVisible(false);
        m_fileButton->setVisible(false);
        m_Combo->setVisible(false);
        m_colorButton->setVisible(false);
        m_dateEdit->setVisible(false);
    }

    setValue(defaultValue);



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



QJBoolean::QJBoolean(QWidget *parent, QJForm *parentForm) :
    QJBase(parent, parentForm)
{
    QHBoxLayout * h  = new QHBoxLayout(this);
    h->setMargin(0);

    QCheckBox   * le = new QCheckBox(this);
    auto s = new ToggleSwitch(8,10,this);

    le->setLayoutDirection(Qt::RightToLeft);
    m_switch = s;
    connect(m_switch, &QAbstractButton::clicked,
    [this](bool ch)
    {
        m_widget->setChecked(ch);
    });

    m_widget = le;

    connect(m_widget, &QAbstractButton::toggled,
    [this]()
    {
        emit getParentForm()->changed();
    });


    h->setAlignment(Qt::AlignLeft);


    h->addWidget( new QWidget(), 1);
    h->addWidget(le, 1);
    h->addWidget(s,1);
    h->update();
}

QJBoolean::~QJBoolean()
{

}
void QJBoolean::setSchema(const QJsonObject &JJ)
{
    QJsonObject J = getParentForm()->dereference(JJ);

    {
        auto mI = J.find("default");
        if( mI != J.end() )
        {
            auto def = mI->toBool(false);
            m_widget->setChecked(def);
            m_switch->setChecked(def);
        }
    }
    QString wid = "checkbox";

    {
        auto mI = J.find("ui:widget");
        if( mI != J.end() )
        {
            wid = mI->toString("checkbox");
        }
    }

    if( wid == "switch")
    {
        m_switch->setVisible(true);
        m_widget->setVisible(false);
    }
    else
    {
        m_switch->setVisible(false);
        m_widget->setVisible(true);
    }
    (void)J;
}

QJsonValue QJBoolean::getValue() const
{
    return m_widget->isChecked();
}


//==============================================================================


//==============================================================================



QJNumber::QJNumber(QWidget *parent, QJForm *parentForm) :
    QJBase(parent, parentForm)
{
    QHBoxLayout * h  = new QHBoxLayout(this);
    h->setMargin(0);
    //this->setLayout(h);

    QDoubleSpinBox   * le = new QDoubleSpinBox(this);
    m_slider = new QSlider(Qt::Horizontal, this);
    m_widget = le;

    h->addWidget(m_slider,10);
    h->addWidget(le, 1);

    connect(m_widget, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                //m_widget,
            [this](double)
            {
                emit getParentForm()->changed();
            }
            );

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
void QJNumber::setSchema(const QJsonObject &JJ)
{
    QJsonObject J = getParentForm()->dereference(JJ);


    double mm = static_cast<double>( std::numeric_limits<int>::lowest() );
    double MM = static_cast<double>( std::numeric_limits<int>::max() );
    double def = 0.0;


    {
        auto mI = J.find("default");
        if( mI != J.end() )
        {
            def = mI->toDouble(0.0);
        }
    }
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

    if( J.find("type")->toString() == "integer" )
    {
        m_widget->setDecimals(0);
    }
}

QJsonValue QJNumber::getValue() const
{
    return dynamic_cast<QDoubleSpinBox*>(m_widget)->value();
}


//==============================================================================


QJArray::QJArray(QWidget *parent, QJForm *parentForm) :
    QJBase(parent, parentForm)
{
    //QFormLayout * h  = new QFormLayout(this);
    //h->setMargin(3);
    //this->setLayout(h);

    {
        QFormLayout * l  = new QFormLayout(this);

        m_propertiesLayout  = new QFormLayout();


        {
            m_oneOf = new QComboBox(this);
        }


        QHBoxLayout * h = new QHBoxLayout();
        h->setAlignment(Qt::AlignRight);
        m_add  = new QToolButton(this); m_add->setText("+");

        connect( m_add, &QToolButton::released,
                 [this]()
        {
           auto i = m_oneOf->currentIndex();
           push_back( m_oneOfArray.at(i).toObject());
           _rebuild();
        });

        h->addWidget(m_oneOf);
        h->addWidget(m_add);

        m_propertiesLayout->setMargin(3);

        l->addRow(m_propertiesLayout);
        l->addRow(h);

        this->setLayout(l);

        {

            m_ListWidget = new QListWidget(this);
            l->addRow(m_ListWidget);
        }

    }
}

void QJArray::push_back(QJsonObject O)
{
    auto * w = new QJValue(this, m_parentForm);
    auto * h = new QHBoxLayout();

    w->setSchema( O );
    h->addWidget(w);

    auto * up   = new QToolButton(this); up->setArrowType(Qt::UpArrow);
    auto * down = new QToolButton(this); down->setArrowType(Qt::DownArrow);
    auto * del  = new QToolButton(this); del->setText("✖");


    up->setVisible(  !m_fixedOrder);
    down->setVisible(!m_fixedOrder);
   // del->setVisible(showButtons);

    up->setMaximumSize( 25,25);
    down->setMaximumSize( 25,25);
    del->setMaximumSize( 25,25);

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
                    emit getParentForm()->changed();
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
                    emit getParentForm()->changed();
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
                emit getParentForm()->changed();
                return;
            }
            ro++;
        }
    });

    h->addWidget( up);
    h->addWidget( down);
    h->addWidget( del);

    m_items.push_back({w,h, up, down, del});
}

QJArray::~QJArray()
{

}

void QJArray::_rebuild()
{
    auto * L =m_propertiesLayout;

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
        x.m_del->setVisible( !m_fixedSize );
        L->addRow( x.m_layout );
    }

}

void QJArray::setSchema(const QJsonObject &JJ)
{
    QJsonObject J = getParentForm()->dereference(JJ);

    m_oneOf->setVisible(false);
    m_add->setVisible(false);
    m_fixedSize=true;

    m_ListWidget->clear();
    m_ListWidget->hide();

    if( J.contains("ui:spacing"))
    {
        auto spacing = J.find("ui:spacing")->toInt();
        m_propertiesLayout->setVerticalSpacing(spacing);
        m_ListWidget->setSpacing(spacing);
    }
    if( J.contains("ui:fixedOrder"))
    {
        m_fixedOrder = J.find("ui:fixedOrder")->toBool();
    }

    if( J.contains("additionalItems"))
    {
        auto aItems = J.find("additionalItems");
        if( aItems->isArray() )
        {
            m_oneOfArray = aItems->toArray();
        }
        else if( aItems->isObject() )
        {
            m_oneOfArray.push_back( aItems->toObject() );
        }
        for(int i=0;i<m_oneOfArray.size();i++)
        {
            QString title = "item_" + QString(i);
            auto obj = m_oneOfArray.at(i).toObject();
            if( obj.contains("title"))
                title = obj.find("title")->toString();
            m_oneOf->addItem( title );
        }

        if( m_oneOfArray.size() >=2)
        {
            m_oneOf->setVisible(true);
        }
        m_add->setVisible(true);
        m_fixedSize=false;
    }

    bool uniqueItems=false;
    if( J.contains("uniqueItems") )
    {
        uniqueItems = J.find("uniqueItems")->toBool();
    }


    if( J.contains("items"))
    {
        auto it = J.find("items");

        if( it->isArray())
        {
            auto p = it->toArray();

            int ro=0;
            for(auto i=p.begin(); i!=p.end();i++)
            {
                push_back( i->toObject() );

                ro++;
            }
        }
        else if (it->isObject())
        {
            auto p = it->toObject();

            if(p.find("type")->toString() == "string" && p.contains("enum") && uniqueItems )
            {
                int i=0;
                for(auto v : p.find("enum")->toArray())
                {
                    m_ListWidget->insertItem(i++, v.toString() );
                }
                m_ListWidget->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
                m_ListWidget->show();
            }
        }

    }
    _rebuild();
}

QJsonValue QJArray::getValue() const
{
    QJsonArray O;
    if( m_ListWidget->count() == 0)
    {
        for(auto & x : m_items)
        {
            O.push_back( x.m_widget->getValue() );
        }
    }
    else
    {
        for(auto x : m_ListWidget->selectedItems()    )
        {
            O.push_back( x->text() );
        }
    }
    return O;
}



//==============================================================================


QJObject::QJObject(QWidget *parent, QJForm *parentForm) :
    QJBase(parent, parentForm)
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

        QHBoxLayout * h = new QHBoxLayout();
        h->setAlignment(Qt::AlignRight);
        h->addWidget(m_oneOf);
        l->addRow(h);
    }


    m_propertiesLayout->setMargin(0);
    m_propertiesLayout->setSpacing(0);
    m_propertiesLayout->setVerticalSpacing(1);

    l->addRow(m_propertiesLayout);
}

QJObject::~QJObject()
{

}

void QJObject::setSchema(const QJsonObject &JJ)
{
    QJsonObject J = getParentForm()->dereference(JJ);

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


void QJObject::setOneOf(const QJsonObject &JJ)
{
    QJsonObject J = getParentForm()->dereference(JJ);

    auto * L = m_propertiesLayout;

    if( m_tabwidget )
    {
        m_tabwidget->deleteLater();
        m_tabwidget=nullptr;
    }

    while(m_propertiesLayout->rowCount())
    {
        m_propertiesLayout->removeRow( m_propertiesLayout->rowCount()-1);
    }
    m_properties.clear();

    std::map< QString, std::pair<QString,QWidget*> > props;

    if( J.contains("ui:spacing"))
    {
        auto spacing = J.find("ui:spacing")->toInt();
        m_propertiesLayout->setVerticalSpacing(spacing);
    }


    if( J.contains("properties"))
    {
        auto p = J.find("properties")->toObject();

        for(auto i=p.begin(); i!=p.end();i++)
        {
            auto k = i.key();
            auto v = i.value();

            auto title=k;

            auto * w = new QJValue(this, getParentForm() );
            auto vO = v.toObject();

            {
                auto t = vO.find("title");
                if( t!=vO.end())
                {
                    title = t->toString( k );
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
                props[k] = {title,w};
            }
            w->setVisible(visible);
            m_properties[ i.key() ] = {title,w};
        }
    }


    QString wid="rows";
    if( J.contains("ui:widget") )
    {
        wid = J.find("ui:widget")->toString();
    }


    if( wid == "tabs")
    {
        m_tabwidget = new QTabWidget(this);
        m_propertiesLayout->addRow(m_tabwidget);
    }


    if( J.contains("ui:order"))
    {
        auto order = J.find("ui:order")->toArray();
        for(auto x : order)
        {
            if( props.count(x.toString()))
            {
                if( wid=="tabs")
                {
                    m_tabwidget->addTab( props.at(x.toString()).second, props.at(x.toString()).first);
                }
                else
                {
                    L->addRow( props.at(x.toString()).first, props.at(x.toString()).second);
                }
            }
            props.erase( x.toString() );
        }
    }


    else
    {
        for(auto & x : props)
        {
            if( wid=="tabs")
            {
                m_tabwidget->addTab( x.second.second, x.second.first);
            }
            else
            {
                L->addRow(x.second.first,x.second.second);
            }
        }
    }
}

QJsonValue QJObject::getValue() const
{
    QJsonObject O;
    for(auto & x : m_properties)
    {
        O[x.first] = x.second.second->getValue();
    }
    return O;
}
















QJValue::QJValue(QWidget *parent, QJForm *parentForm) :
    QJBase(parent, parentForm)
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
    else if( auto * w = dynamic_cast<QJBoolean*>(m_widget))
    {
        return w->getValue();
    }
    return {};
}

void QJValue::setSchema(const QJsonObject &JJ)
{
    try{
        if( m_widget )
            delete m_widget;

        QJsonObject J = getParentForm()->dereference(JJ);

        auto type = J.find("type")->toString();

        if( type == "string" )
        {
            auto * w = new QJString(this, getParentForm());
            m_widget = w;
            w->setSchema(J);
            this->layout()->addWidget(m_widget);
        }
        else if( type == "number" || type == "integer")
        {
            auto * w = new QJNumber(this, getParentForm());
            m_widget = w;
            w->setSchema(J);
            this->layout()->addWidget(m_widget);
        }
        else if( type == "object")
        {
            auto * w = new QJObject(this, getParentForm());
            m_widget = w;
            w->setSchema(J);
            this->layout()->addWidget(m_widget);
        }
        else if( type == "array")
        {
            auto * w = new QJArray(this, getParentForm());
            m_widget = w;
            w->setSchema(J);
            this->layout()->addWidget(m_widget);
        }
        else if( type == "boolean")
        {
            auto * w = new QJBoolean(this, getParentForm());
            m_widget = w;
            w->setSchema(J);
            this->layout()->addWidget(m_widget);
        }

    }
    catch ( std::exception & e) {
        std::cerr << e.what() << std::endl;
        throw e;
    }
    this->updateGeometry();

}


QJValue::~QJValue()
{
}


void QJString::setValue(QString S)
{
    if( m_Combo)
    {
        for(int i=0;i< m_Combo->count();i++)
        {
            if( m_Combo->itemText(i) == S )
            {
                m_Combo->setCurrentIndex(i);
                break;
            }
        }
    }
    if( m_dateEdit->isVisible())
    {
        QDate d;
        d = QDate::fromString(S, Qt::ISODate);
        m_dateEdit->setDate(d);
    }
    else
    {
        if(m_widget)
        {
            m_widget->setText(S);
        }
    }
}

void QJBoolean::setValue(bool b)
{
    if( m_switch)
    {
        m_switch->setChecked(b);
    }
    if(m_widget)
    {
        m_widget->setChecked(b);
    }
}


void QJNumber::setValue(double b)
{
    m_widget->setValue(b);
    double t = static_cast<double>(b - m_widget->minimum()) / static_cast<double>( m_widget->maximum() - m_widget->minimum());
    double R = m_slider->minimum() * (1.0-t) + m_slider->maximum() * t;
    m_slider->blockSignals(true);
    m_slider->setValue( static_cast<int>(R) );
    m_slider->blockSignals(false);

}

void QJArray::setValue(QJsonArray A)
{
    auto mm = std::min( static_cast<size_t>(A.size()), m_items.size() );
    for(size_t i=0;i<mm;i++)
    {
        m_items[i].m_widget->setValue( A[i] );
    }
}

void QJObject::setValue(QJsonObject A)
{
    for(auto k : A.keys())
    {
        if( m_properties.count(k))
        {
            m_properties.at(k).second->setValue( A[k]);
        }
    }
}

void QJValue::setValue(QJsonValue J)
{
    if( auto * w = dynamic_cast<QJNumber*>(m_widget))
    {
        if( J.isDouble())
            w->setValue( J.toDouble());
    }
    else if( auto * w = dynamic_cast<QJString*>(m_widget))
    {
        if( J.isString())
            w->setValue(J.toString());
    }
    else if( auto * w = dynamic_cast<QJObject*>(m_widget))
    {
        if( J.isObject())
            w->setValue(J.toObject());
    }
    else if( auto * w = dynamic_cast<QJArray*>(m_widget))
    {
        if( J.isArray())
            w->setValue( J.toArray() );
    }
    else if( auto * w = dynamic_cast<QJBoolean*>(m_widget))
    {
        if( J.isBool())
            w->setValue(J.toBool());
    }
}











QJForm::QJForm(QWidget *parent) : QWidget(parent)
{
    auto vb       = new QVBoxLayout(this);
    m_scrollArea  = new QScrollArea(this);

    vb->setMargin(0);
    vb->addWidget( m_scrollArea );


}

QJsonObject QJForm::get() const
{
    if( m_jvalue)
        return m_jvalue->getValue().toObject();
    return {};
}

QJsonObject QJForm::getDef(QString ref) const
{
    std::string r = ref.toStdString();
    if( r.size() > 2)
    {
        if( r.front() == '#' && r[1] =='/')
        {
            return getRef(m_schema, std::string( r.begin()+2, r.end()));
        }
    }
    return {};
}

QJsonObject QJForm::dereference(QJsonObject JJ) const
{
    QJsonObject J;
    if( JJ.contains("$ref") )
    {
        J = getDef( JJ.find("$ref")->toString() );

        for(auto i : JJ.keys())
        {
            J[i] = JJ[i];
        }
    }
    else
    {
        J = JJ;
    }
    return J;
}

void QJForm::setSchema(const QJsonObject &J)
{
    m_schema = J;

    if( m_jvalue)
        delete m_jvalue;

    m_jvalue = new QJValue(this, this);
    m_jvalue->setSchema(J);
    m_scrollArea->setWidget(m_jvalue);
    m_scrollArea->setWidgetResizable(true);
}

void QJForm::setValue(QJsonObject const &J)
{
    QJsonValue V(J);
    m_jvalue->setValue( V );
}

}
