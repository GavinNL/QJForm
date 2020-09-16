#include <iostream>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QJForm/qjform.h>

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

};

static QJsonObject ObjectFromString(const QString& in)
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

    //auto * x = new QJValue(this);
    //x->setSchema(J);


    uint32_t bCount=0;
    std::string jsonStr;
    while( !std::cin.eof() )
    {
        char c;
        std::cin >> c;
        if( c == '{') ++bCount;
        if( c == '}') --bCount;
        jsonStr += c;
        if( bCount == 0 )
        {
            break;
        }
    }

    QJsonObject J = ObjectFromString( QString(jsonStr.c_str()) );

    auto centralWidget = new QWidget(this);

    auto layout = new QVBoxLayout();
    centralWidget->setLayout( layout );
    this->setCentralWidget(centralWidget);

    auto W  = new QJForm::QJForm(centralWidget);
    W->setSchema(J);

    auto Ok = new QPushButton(this);
    Ok->setText("Ok");
    layout->addWidget(W);
    layout->addWidget(Ok);

    W->setContentsMargins(0,0,0,0);

    if( J.contains("title"))
    {
        this->setWindowTitle( J.find("title")->toString());
    }


    connect( Ok, &QPushButton::clicked,
             [W](bool)
            {
                QByteArray ba = QJsonDocument( W->get()).toJson();
                std::cout << ba.toStdString() << std::endl;
            });

    //connect( W, &QJForm::QJForm::changed,
    //         [W]()
    //        {
    //         QByteArray ba = QJsonDocument( W->get()).toJson();
    //         std::cout << ba.toStdString() << std::endl;
    //        });
}

MainWindow::~MainWindow()
{

}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.show();
    return a.exec();
}



