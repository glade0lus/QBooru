#ifndef SEARCH_TAB_H
#define SEARCH_TAB_H

#include <QtWidgets>

#include "imageTabClass.h"

#include "../BooruHeaders/headers.h"

#include "../classBooruSite.h"
#include "../Workers/caching_thread.h"
#include "../Workers/image_loading_thread.h"

class Widget;
class ImageTab;
class CachingWorker;
class SearchTabLoadingWorker;

class SearchTab : public QWidget
{
    Q_OBJECT

public:
    SearchTab(Widget *parent, BooruSite site);
    ~SearchTab();

    void initialisationMiniatures();
    void loadTagViewer(QString tagString);
    void loadSearch(int refreshTags);

    void on_pushButton_pageMoins_clicked();
    void on_pushButton_pagePlus_clicked();
    void setKeyboard();
    void checkPageButtonStatus();
    void setRating(const char* rating);

    /*Objets*/
    BooruImage images[64];
    BooruSite booru;

    QVBoxLayout *layoutSearchStatus;
        QLineEdit *lineEditTags;
        QProgressBar *progressBarSearch;
        QLabel *labelSearchStatus;

        QThread *thread_pool[64];
        CachingWorker *caching_worker_pool[64];

        QThread *thread_pool_loading[64];
        SearchTabLoadingWorker *loading_worker[64];
        ImageTab *imageTabs[64];

        int picture_number;
        int picture_rows;
        int picture_columns;


protected:
    bool eventFilter(QObject *object, QEvent *event);

signals:

public slots:
    void searchActionToggled(int sender);
    void setViewer(int sender);
    void newSearch();
    void startLoadingPicture(int i);
    void image_loaded();

private:
    int loaded_pictures;
    int recherche;
    int progress;

    std::string derpibooruAPIKey;
    QString tagsSearched;

    QSignalMapper *signalMapper;
    QSignalMapper *buttonMapper;

    bool isAffiched[64];

    Widget *parentWidget;

    QVBoxLayout *layoutMain;

        QHBoxLayout *layoutSearch;
        QHBoxLayout *layoutTags;

            QPushButton *searchButton;



            QHBoxLayout *layoutSearchRating;
            QComboBox *searchRating;

        QGridLayout *layoutImageTabs;


        QHBoxLayout *layoutSearchPage;
            QPushButton *pushButtonPageMoins;
            QLineEdit *lineEditPageSet;
            QPushButton *pushButtonPagePlus;

};

#endif // SEARCH_TAB_H
