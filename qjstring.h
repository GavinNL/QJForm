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

#include <QFormLayout>

class QJValue;

class QJBase : public QWidget
{
public:
    explicit QJBase(QWidget *parent = nullptr);

    virtual void setSchema(const QJsonObject & J) = 0;
    virtual QJsonValue getValue() const = 0;
};


class QJString : public QJBase
{
    Q_OBJECT

public:
    explicit QJString(QWidget *parent = nullptr);


    void setSchema(const QJsonObject & J) override;
    QJsonValue getValue() const override;

    ~QJString();

    QComboBox * m_Combo  = nullptr;
    QLineEdit * m_widget  = nullptr;
    QPushButton * m_fileButton = nullptr;
    QPushButton * m_dirButton = nullptr;

};

class QJNumber : public QJBase
{
    Q_OBJECT

public:
    explicit QJNumber(QWidget *parent = nullptr);

    QJsonValue getValue() const override;

    void setSchema(const QJsonObject & J) override;
    ~QJNumber();

private:
    QDoubleSpinBox * m_widget = nullptr;
    QSlider        * m_slider = nullptr;

};


class QJObject : public QJBase
{
    Q_OBJECT

public:
    explicit QJObject(QWidget *parent = nullptr);

    QJsonValue getValue() const override;

    void setOneOf(const QJsonObject & J);
    void setSchema(const QJsonObject & J) override;
    ~QJObject();
    std::map< QString, QJValue*> m_properties;

    QJsonArray m_oneOfArray;
    QComboBox * m_oneOf=nullptr;
    QFormLayout * m_propertiesLayout = nullptr;

};


class QJArray : public QJBase
{
    Q_OBJECT

public:
    explicit QJArray(QWidget *parent = nullptr);

    QJsonValue getValue() const override;

    void setSchema(const QJsonObject & J) override;
    ~QJArray();
    std::vector< QJValue*> m_items;

};





class QJValue : public QWidget
{
    Q_OBJECT

public:
    explicit QJValue(QWidget *parent = nullptr);

    QJsonValue getValue() const;

    ~QJValue();

    void setSchema(QJsonObject const & J);


private:
    QWidget * m_widget=nullptr;

};

#endif // QJSTRING_H
