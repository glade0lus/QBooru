#include "widget.h"
#include "fonctions.h"
#include "classBooruSite.h"
#include <QApplication>
#include <QIcon>
#include <QMessageBox>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <QMessageBox>

#include "fonctions.h"

#include "Widget/imageTabClass.h"
#include "Widget/optionTabClass.h"
#include "Widget/searchTabClass.h"
#include "Widget/viewerTabClass.h"

using namespace std;

int main(int argc, char *argv[])
{
    srand(time(NULL));
    QApplication *a = new QApplication(argc, argv);
    std::freopen(LOG_FILE, "a", stdout);

    clog << endl;

    std::cout << std::endl;

    cachingFile(CONF_FILE_URL, CONF_FILE, false, true);
    setAppVersionJSON();
    checkConfigFile();

    QIcon icone;
    icone.addFile(ICON_PATH);


    QMessageBox* msgBox = new QMessageBox();

    outputInfo("INFO",std::string("Generating Widget"),LEVEL_TOP_WIDGET);
    Widget w(msgBox);

    w.setWindowTitle(QString(APP_NAME) + QString(" ") + QString(OS_ID) + QString(" ") + QString(APP_VERSION));
    outputInfo("INFO",std::string("Showing Widget"),LEVEL_TOP_WIDGET);
    w.show();

    int ret = a->exec();
    w.deleteLater();
    a->deleteLater();

    return ret;
}
