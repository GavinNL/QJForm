#ifndef QJSTRING_H
#define QJSTRING_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QScrollArea>
#include <QFormLayout>
#include <QCheckBox>

namespace QJForm
{


class QJValue;
class QJForm;

class QJBase : public QWidget
{
public:
    explicit QJBase(QWidget *parent, QJForm *parentForm);

    virtual void setSchema(const QJsonObject & J) = 0;
    virtual QJsonValue getValue() const = 0;

    QJForm* getParentForm()
    {
        return m_parentForm;
    }
    QJForm const * getParentForm() const
    {
        return m_parentForm;
    }
protected:
    QJForm *m_parentForm = nullptr;
};


class QJString : public QJBase
{
    Q_OBJECT

public:
    explicit QJString(QWidget *parent, QJForm *parentForm);


    void setSchema(const QJsonObject & J) override;
    QJsonValue getValue() const override;

    ~QJString();

    QComboBox * m_Combo  = nullptr;
    QLineEdit * m_widget  = nullptr;
    QPushButton * m_fileButton = nullptr;
    QPushButton * m_dirButton = nullptr;
    QPushButton * m_colorButton = nullptr;

};

class QJBoolean : public QJBase
{
    Q_OBJECT

public:
    explicit QJBoolean(QWidget *parent, QJForm *parentForm);

    QJsonValue getValue() const override;

    void setSchema(const QJsonObject & JJ) override;
    ~QJBoolean();

private:
    QCheckBox * m_widget = nullptr;
    QAbstractButton * m_switch=nullptr;

};

class QJNumber : public QJBase
{
    Q_OBJECT

public:
    explicit QJNumber(QWidget *parent, QJForm *parentForm);

    QJsonValue getValue() const override;

    void setSchema(const QJsonObject & JJ) override;
    ~QJNumber();

private:
    QDoubleSpinBox * m_widget = nullptr;
    QSlider        * m_slider = nullptr;

};


class QJObject : public QJBase
{
    Q_OBJECT

public:
    explicit QJObject(QWidget *parent, QJForm *parentForm);

    QJsonValue getValue() const override;

    void setOneOf(const QJsonObject & J);
    void setSchema(const QJsonObject & JJ) override;
    ~QJObject();
    std::map< QString, std::pair<QString,QJValue*>> m_properties;

    QJsonArray m_oneOfArray;
    QComboBox * m_oneOf=nullptr;
    QFormLayout * m_propertiesLayout = nullptr;

};


class QJArray : public QJBase
{
    Q_OBJECT

public:
    explicit QJArray(QWidget *parent, QJForm *parentForm);

    QJsonValue getValue() const override;

    void setSchema(const QJsonObject & JJ) override;
    ~QJArray();

    struct item_t
    {
        QJValue * m_widget;
        QHBoxLayout * m_layout;
        QToolButton *m_up;
        QToolButton *m_down;
        QToolButton *m_del;
    };

    std::vector< item_t > m_items;

    void _rebuild();

    void push_back( QJsonObject O);
    QJsonArray m_oneOfArray;
    QComboBox * m_oneOf=nullptr;
    QToolButton * m_add=nullptr;
    QFormLayout * m_propertiesLayout = nullptr;
    bool m_fixedSize=false;

};





class QJValue : public QJBase
{
    Q_OBJECT

public:
    explicit QJValue(QWidget *parent, QJForm *parentForm);

    QJsonValue getValue() const;

    ~QJValue();

    void setSchema(QJsonObject const & J);


private:
    QWidget * m_widget=nullptr;

};




class QJForm : public QWidget
{
    Q_OBJECT

public:
    explicit QJForm(QWidget *parent = nullptr);

    ~QJForm(){};

    void setSchema(QJsonObject const & J);


    QJsonObject dereference(QJsonObject JJ) const;

    /**
     * @brief getDef
     * @param ref
     * @return
     *
     * Gets an object using the following reference
     *
     * ref = "#/key1/key2/key3"
     */
    QJsonObject getDef(QString ref) const;

    QJsonObject const& getSchema() const
    {
        return m_schema;
    }
    QJsonObject get() const;
Q_SIGNALS:
    void changed();

private:
    QScrollArea * m_scrollArea=nullptr;
    QJValue     * m_jvalue=nullptr;
    QJsonObject   m_schema;

};


}

#endif // QJSTRING_H
