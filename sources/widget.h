#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include <qtconcurrentrun.h>
#include <QThread>

/*
#include <QDebug>
#include <QDir>
#include <QPixmap>
#include <QProcess>
#include <QWidget>
#include <QPushButton>
#include <QSignalMapper>
#include <QTabWidget>
#include <QMessageBox>
*/

#include "BooruHeaders/derpibooru_recherche.h"
#include "BooruHeaders/derpibooru_fonctions.h"

#include "BooruHeaders/gelbooru_recherche.h"
#include "BooruHeaders/gelbooru_fonctions.h"

#include "BooruHeaders/moebooru_recherche.h"
#include "BooruHeaders/moebooru_fonctions.h"

#include "Widget/imageTabClass.h"
#include "Widget/optionTabClass.h"
#include "Widget/searchTabClass.h"
#include "Widget/viewerTabClass.h"
#include "Widget/booruSettingsTabClass.h"
#include "Widget/loadingWidget.h"

#include "classBooruSite.h"

class SearchTab;
class ViewerTab;
class OptionTab;
class BooruSettingsTab;
class LoadingWidget;


class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QMessageBox *msgBox, QWidget *parent = 0);
    ~Widget();
    void setupHosts(BooruSite boorus[], int index);
    void loadTag(QString tag, int imageHostInt);
    void setTab(int tab);

    /*Public Objects*/
    int booruNumber;
    int viewerIndex;

    ViewerTab *viewerTab;
    SearchTab *searchtabs[16];
    QLabel *loadingLabel;

public slots:
    void on_tab_changed(int tabIndex);
    void refresh();

private:
    QVBoxLayout *mainLayout; 
    OptionTab *optionTab;
    QTabWidget *tabWidgetMain;
    BooruSettingsTab *settingTabs;

    QString hostNames[16];

    QThread *thread[16];
};

#endif // WIDGET_H
