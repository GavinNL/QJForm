#include "mainwindow.h"
#include "qjstring.h"
#include <QHBoxLayout>
#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>

QJsonObject ObjectFromString(const QString& in)
{
    QJsonObject obj;

    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

    // check validity of the document
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            obj = doc.object();
        }
        else
        {
            return {};
        }
    }
    else
    {
        return {};
    }

    return obj;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto * x = new QJValue(this);

    this->setCentralWidget(x);

    auto jstr = R"foo({
    "type" : "object",
    "properties" : {
        "str" : { "type" : "string", "default" : "hello world" },
        "enum" : { "type" : "string", "default" : "hello world", "enum" : ["first", "second", "third" ] },
        "file" : { "type" : "string", "default" : "/", "ui:widget" : "file" },
        "dir" : { "type" : "string", "default" : "/", "ui:widget" : "dir" },
        "num" : { "type" : "number", "minimum" : 0, "maximum" : 50, "default" : 10 },
        "range" : { "type" : "number", "minimum" : 0, "maximum" : 50, "default" : 10, "ui:widget" : "range" },
        "obj" : {
                "type" : "object",
                "properties" : {
                    "str" : { "type" : "string", "title" : "String" },
                    "num" : { "type" : "number" }
                }
            },
        "arr" : {
                "type" : "array",
                "items" : [
                    { "type" : "string", "title" : "String" },
                    { "type" : "number" },
            { "type" : "number" , "minimum" : 0, "maximum" : 100},
            { "type" : "number" , "minimum" : 0, "maximum" : 100},
            { "type" : "number" , "minimum" : 0, "maximum" : 100, "ui:widget" : "range" }
                ]
            }
    }
}
)foo";

    // Get JSON object
    QJsonObject J = ObjectFromString( QString(jstr) );

    x->setSchema(J);

    // `ba` contains JSON
     auto root = x->getValue().toObject();
     QByteArray ba = QJsonDocument(root).toJson();
     std::cout << ba.toStdString() << std::endl;
}

MainWindow::~MainWindow()
{

}

