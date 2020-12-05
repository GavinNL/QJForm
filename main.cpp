#include <iostream>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QCommandLineParser>
#include <QJForm/qjform.h>

bool asBash=false;

void printJsonBash(QJsonValue J, std::string prefix)
{
    if( J.isObject() )
    {
        auto Jo = J.toObject();
        for(auto key : Jo.keys() )
        {
            auto val = Jo.find(key);
            if( val->isString() )
            {
                std::cout << prefix << key.toStdString() << "=\"" << val->toString().toStdString() << "\"" << std::endl;
            }
            else if( val->isDouble() )
            {
                std::cout << prefix << key.toStdString() <<  "=" << val->toDouble() << std::endl;
            }
            else if( val->isBool())
            {
                std::cout << prefix << key.toStdString() <<  "=" << (val->toBool() ? std::string("true") : std::string("false")) << std::endl;
            }
            else
            {
                auto new_prefix = prefix + key.toStdString() + "_";
                printJsonBash( *val, new_prefix);
            }
        }
    }
    else if (J.isArray())
    {
        uint32_t i=0;
        auto Ja = J.toArray();
        for(auto val : Ja)
        {
            if( val.isString() )
            {
                std::cout << prefix << i << "=\"" << val.toString().toStdString() << "\"" << std::endl;
            }
            else if( val.isDouble() )
            {
                std::cout << prefix << i <<"=" << val.toDouble() << std::endl;
            }
            else if( val.isBool())
            {
                std::cout << prefix << i << "=" << ( val.toBool() ? std::string("true") : std::string("false")) << std::endl;
            }
            else
            {
                auto new_prefix = prefix + std::to_string(i) + "_";
                printJsonBash(val, new_prefix);
            }
            i++;
        }
    }
}

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
            throw std::runtime_error("Failed to parse");
            return {};
        }
    }
    else
    {
        throw std::runtime_error("Failed to parse");
        return {};
    }

    return obj;
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

//#define RUN_QJFORM_TEST

#ifndef RUN_QJFORM_TEST

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
#else
    auto jstr = R"foo({
    "type" : "object",
    "title" : "Json Example",
    "ui:order" : ["str", "enum", "file", "dir", "num", "range", "arr", "objectTabs"],
    "properties" : {
        "str" :  { "type" : "string", "default" : "hello world", "description" : "This is a tooltip", "title" : "Strings" },
        "enum" : { "type" : "string", "enum" : ["first", "second", "third" ] },
        "num" :  { "type" : "number", "minimum" : 0, "maximum" : 50, "default" : 10 },
        "arr" : {
                "type" : "array",
                "items" : [
                    { "type" : "string", "title" : "String" },
                    { "type" : "number" },
                    { "type" : "number" , "minimum" : 0, "maximum" : 100},
                    { "type" : "number" , "minimum" : 0, "maximum" : 100},
                    { "type" : "number" , "minimum" : 0, "maximum" : 100, "ui:widget" : "range" }
                ]
            },
        "objectTabs" : {
                "type" : "object",
                "ui:widget" : "tabs",
                "properties" : {

                    "Numbers" : {
                            "type" : "object",
                            "properties" : {

                                "num" :  { "type" : "number", "minimum" : 0, "maximum" : 50, "default" : 10 },
                                "slider" :  { "type" : "number", "minimum" : 0, "maximum" : 50, "default" : 10, "ui:widget" : "range" }

                            }
                    },
                    "Strings" : {
                            "type" : "object",
                            "properties" : {

                                "str" :  { "type" : "string", "default" : "hello world", "description" : "This is a tooltip", "title" : "Strings" },
                                "enum" : { "type" : "string", "enum" : ["first", "second", "third" ] }

                            }
                    }

                }
        }
    }
}
)foo";

    auto jstr_values = R"foo({
    "str" : "testing passed",
    "enum" : "third",
    "num" : 28,
    "arr" : [ "test passed", 1,3,5,7]
    }
)foo";

    QJsonObject J = ObjectFromString( QString(jstr) );
    QJsonObject Jv = ObjectFromString( QString(jstr_values) );
#endif

    auto centralWidget = new QWidget(this);

    auto layout = new QVBoxLayout();
    centralWidget->setLayout( layout );
    this->setCentralWidget(centralWidget);

    auto W  = new QJForm::QJForm(centralWidget);
    W->setSchema(J);
    #ifdef RUN_QJFORM_TEST
    W->setValue(Jv);
    #endif

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

                if( asBash)
                {
                    printJsonBash( QJsonValue(W->get()) , "export " );
                }
                else
                {
                    std::cout << ba.toStdString() << std::endl;
                }
            });
}

MainWindow::~MainWindow()
{

}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("QJForm");
    parser.addHelpOption();

    // A boolean option with a single name (-p)
    QCommandLineOption showProgressOption("b", QCoreApplication::translate("main", "Print output as bash variables instead of json"));
    QCommandLineOption showVersion("v", QCoreApplication::translate("main", "Print the version"));
    parser.addOption(showProgressOption);
    parser.addOption(showVersion);

    // Process the actual command line arguments given by the user
    parser.process(a);

    const QStringList args = parser.positionalArguments();
    // source is args.at(0), destination is args.at(1)

    asBash = parser.isSet(showProgressOption);

    if( parser.isSet(showVersion))
    {
        std::cout << QJFORM_VERSION << std::endl;
        return 0;
    }
    MainWindow w;
    w.show();
    return a.exec();
}



