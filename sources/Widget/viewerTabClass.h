#ifndef VIEWERTABCLASS_H
#define VIEWERTABCLASS_H

#include <QtWidgets>

#include "constantes.h"
#include "../widget.h"

#include "../BooruHeaders/headers.h"

#include "../classBooruSite.h"

#include "../Workers/viewer_loading_thread.h"

class ViewerTabLoadingWorker;

class ViewerTab: public QWidget
{
    Q_OBJECT

public:
    ViewerTab(Widget *parent = 0);
    ~ViewerTab();

    void loadLabelsTags(std::string tagsArray[TAG_NUMBER], int tagNumber);

    void loadPicture(int imageHostInt, int tabIndex, int index, int pageIndexInt, BooruSite booruSite);
    void loadPicture();

    bool eventFilter(QObject *object, QEvent *event);
    void setKeyboard();

    /*Public objects*/
    BooruImage Image;

    BooruSite booru;

    QLabel *labelLoading;
    QLabel *labelImage;

    QProgressBar *progressBar;
    QSize thumbZoneSize;
    QString imageViewer;

public slots:
    void buttonClicked();
    void on_tagSelected(QString tag);
    void downloadPicture();
    void viewFullscreen();
    void showPreviousPicture();
    void showNextPicture();
    void startLoadingPicture();
    void imageLoaded();


private:
    Widget *parentWidget;

    QString fullImagePath;
    char* currentTagList;

    QHBoxLayout *layoutMain;


        QVBoxLayout *layoutInfoTagsSeparator;

            QScrollArea *scrollAreaTags;
                QWidget *scrollAreaTagsContents;
                    QVBoxLayout *layoutTags;
                        QPushButton *tags[TAG_NUMBER];

            QGroupBox *groupBoxInfo;
                QVBoxLayout *layoutGroupBox;
                    QLabel *labelInfo[16];

            QPushButton *pushButtonFullscreen;
            QPushButton *pushButtonDownload;

            QHBoxLayout *layoutSwitchPicture;
                QPushButton *pushButtonSwitchMoins;
                QPushButton *pushButtonSwitchPlus;

            QLabel *labelPage;

    int imageHost;
    int imageIndex;
    int pageIndex;
    QThread *picture_thread;
    ViewerTabLoadingWorker *worker;

};

#endif // VIEWERTABCLASS_H
