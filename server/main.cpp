#include "qlipmon.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QtDebug>

class Application: public QApplication {
private:
    QlipMon *qlipmon;

public:
    Application(int argc, char* argv[]):
        QApplication(argc, argv),
        qlipmon(nullptr)
    {

        setApplicationName("QlipMon");
        setApplicationVersion("1.0");

        Config config;
        config.loadArgs(argc, argv);
        qlipmon = new QlipMon(config);


    }

    ~Application(){
        if (qlipmon != nullptr)
            delete qlipmon;
    }
};

int main(int argc, char *argv[])
{
    Application app(argc, argv);
    return app.exec();
}
