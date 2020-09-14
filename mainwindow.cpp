#include "mainwindow.h"
#include "qjstring.h"
#include <QHBoxLayout>
#include <iostream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollArea>

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


    //this->setCentralWidget(x);

    auto jstr = R"foo({
    "type" : "object",
    "title" : "Json Example",
    "ui:order" : ["str", "enum", "file", "dir", "num", "range", "arr", "obj"],
    "properties" : {
        "invisible" :  { "ui:visible" : false, "type" : "string", "default" : "hello world" },
        "str" :  { "type" : "string", "default" : "hello world", "description" : "This is a tooltip" },
        "enum" : { "type" : "string", "default" : "hello world", "enum" : ["first", "second", "third" ] },
        "file" : { "type" : "string", "default" : "/", "ui:widget" : "file" },
        "dir" :  { "type" : "string", "default" : "/", "ui:widget" : "dir" },
        "num" :  { "type" : "number", "minimum" : 0, "maximum" : 50, "default" : 10 },
        "range" : { "type" : "number", "minimum" : 0, "maximum" : 50, "default" : 10, "ui:widget" : "range" },
        "obj" : {
                "type" : "object",
                "oneOf" : [
                    {
                        "type" : "object",
                        "title" : "First object",
                        "properties" : {
                            "str" : { "type" : "string", "title" : "String" },
                            "num" : { "type" : "number" }
                        }
                    },
                    {
                        "type" : "object",
                        "title" : "Second Object",
                        "properties" : {
                            "str" : { "type" : "string", "default" : "hello world" },
                            "enum" : { "type" : "string", "default" : "hello world", "enum" : ["first", "second", "third" ] },
                            "file" : { "type" : "string", "default" : "/", "ui:widget" : "file" },
                            "dir" : { "type" : "string", "default" : "/", "ui:widget" : "dir" },
                            "num" : { "type" : "number", "minimum" : 0, "maximum" : 50, "default" : 10 },
                            "range" : { "type" : "number", "minimum" : 0, "maximum" : 50, "default" : 10, "ui:widget" : "range" }
                        }
                    }
                ],
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
    auto * x = new QJValue(this);
    x->setSchema(J);


    auto W  = new QJForm();
    W->setSchema(J);

    this->setCentralWidget(W);

    if( J.contains("title"))
    {
        this->setWindowTitle( J.find("title")->toString());
    }


    connect( W, &QJForm::update,
             [](QJsonObject root)
            {
             QByteArray ba = QJsonDocument(root).toJson();
             std::cout << ba.toStdString() << std::endl;
            });
}

MainWindow::~MainWindow()
{

}

