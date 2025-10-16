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

        setApplicationName(APP_NAME);
        setApplicationVersion(APP_VERSION);

        Config::createFromCLI(argc, argv);
        qlipmon = new QlipMon();

        qInfo()<<"Application name: " << applicationName();
        qInfo()<<"Application version: " << applicationVersion();
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
