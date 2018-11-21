#include "viewerTabClass.h"
#include "../fonctions_all.h"
#include "../constantes.h"
#include "../widget.h"
#include "../classBooruSite.h"

ViewerTab::ViewerTab(Widget *parent) : QWidget(parent)
{    
    int i;
    installEventFilter(this);

    isLoading = false;

    /*Déclaration des variables (avec arbre)*/
    parentWidget = parent;

    labelLoading = new QLabel(tr("Waiting"),this);
    progressBar = new QProgressBar(this);

    layoutMain = new QHBoxLayout;

        labelImage = new QLabel(tr("Search an image first"));

        layoutInfoTagsSeparator = new QVBoxLayout;

            scrollAreaTags = new QScrollArea;
                scrollAreaTagsContents = new QWidget;
                    layoutTags = new QVBoxLayout;

            groupBoxInfo = new QGroupBox(tr("Info"));
                layoutGroupBox = new QVBoxLayout;
                    for(i=0;i<16;i++)
                    {
                        labelInfo[i] = new QLabel(this);
                        labelInfo[i]->setText(tr("Some Text"));
                    }
            pushButtonDownload = new QPushButton(tr("Download"));

            layoutSwitchPicture = new QHBoxLayout;
                pushButtonSwitchMoins = new QPushButton("<--");
                pushButtonSwitchPlus = new QPushButton("-->");

            labelPage = new QLabel(tr("Search Page"));
            labelPage->setStyleSheet("qproperty-alignment: AlignCenter;");

    /*Assignation des éléments aux layouts*/

        /*Picture Switch*/
        layoutSwitchPicture->addWidget(pushButtonSwitchMoins);
        layoutSwitchPicture->addWidget(pushButtonSwitchPlus);

        /*Misc Buttons*/


        /*GroupBoxInfo*/
        for(i=0;i<16;i++)
        {
            layoutGroupBox->addWidget(labelInfo[i]);
        }
        groupBoxInfo->setLayout(layoutGroupBox);

        /*ScrollAreaTags*/
        /*
        for(i=0;i<tags.size();i++)
        {
            layoutTags->addWidget(tags.at(i));
        }
        */
        scrollAreaTagsContents->setLayout(layoutTags);
        scrollAreaTags->setWidget(scrollAreaTagsContents);

    /*Right Separator*/
    layoutInfoTagsSeparator->addWidget(scrollAreaTags);
    layoutInfoTagsSeparator->addWidget(groupBoxInfo);
    layoutInfoTagsSeparator->addWidget(progressBar);
    layoutInfoTagsSeparator->addWidget(labelLoading);
    layoutInfoTagsSeparator->addWidget(pushButtonDownload);
    layoutInfoTagsSeparator->addLayout(layoutSwitchPicture);
    layoutInfoTagsSeparator->addWidget(labelPage);

    /*Main layout*/

    labelImage->setAlignment(Qt::AlignCenter);

    layoutMain->addStretch(1);
    layoutMain->addWidget(labelImage);
    layoutMain->addStretch(1);
    layoutMain->addLayout(layoutInfoTagsSeparator);

    labelImage->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    labelImage->setMinimumSize(700,700);
    scrollAreaTags->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setLayout(layoutMain);

    connect(pushButtonDownload,SIGNAL(clicked()),this,SLOT(downloadPicture()));
    connect(pushButtonSwitchMoins,SIGNAL(clicked()),this,SLOT(showPreviousPicture()));
    connect(pushButtonSwitchPlus,SIGNAL(clicked()),this,SLOT(showNextPicture()));

}

ViewerTab::~ViewerTab()
{
    //clearLayoutSecond(layoutMain);
}

void ViewerTab::loadLabelsTags(std::vector<std::string> tagsArray)
{
    int i;
    clearLayout(layoutTags);
    delete layoutTags;
    delete scrollAreaTagsContents;
    scrollAreaTagsContents = new QWidget;
    layoutTags = new QVBoxLayout;

    tags.clear();

    for(i=0;i<tagsArray.size();i++)
    {
        tags.push_back( new QPushButton(QString(tagsArray.at(i).c_str()),this));
        connect(tags.at(i), SIGNAL(pressed()), this, SLOT(buttonClicked()));
        tags.at(i)->setFlat(true);
        tags.at(i)->setStyleSheet ("text-align: left");
        layoutTags->addWidget(tags.at(i));
    }


    layoutTags->setSpacing(0);
    scrollAreaTagsContents->setLayout(layoutTags);
    scrollAreaTags->setWidget(scrollAreaTagsContents);
}

void ViewerTab::buttonClicked()
{
    QPushButton *button = (QPushButton *)sender();
    emit on_tagSelected(button->text());
}

void ViewerTab::on_tagSelected(QString tag)
{
    parentWidget->loadTag(tag, booru->getIndex());
    parentWidget->setTab(booru->getIndex());
}

void ViewerTab::loadPicture(int imageHostInt, int tabIndex, int index, int pageIndexInt, BooruSite *booruSite)
{
    //std:: cout << imageHostInt << tabIndex << index << pageIndexInt << booruSite.name << std::endl;
    booru = booruSite;
    imageHost = imageHostInt;
    imageIndex = index;
    pageIndex = pageIndexInt;

    thumbZoneSize.setHeight(this->height());
    thumbZoneSize.setWidth(this->width()-scrollAreaTags->width());

    progressBar->setValue(0);
    labelLoading->setText(tr("Loading Picture"));

    labelPage->setText(QString(tr("Search Page : ")) + QString::number(pageIndex));

    clearLayout(layoutGroupBox);

    Image = parentWidget->searchtabs[tabIndex]->images[imageIndex];
    loadPicture();
    progressBar->setValue(50);
    labelLoading->setText(tr("Loading tags"));
    if(booru->getSiteTypeInt() != DERPIBOORU_TYPE)
    {
        loadLabelsTags(Image.tags);
    }

    progressBar->setValue(50);
    labelLoading->setText(tr("Loading picture"));

    startLoadingPicture();

    parentWidget->setTab(parentWidget->viewerIndex);
}

void ViewerTab::loadPicture()
{
    QString infos[16];
    std::string date;

    date = Image.created_at;

    infos[0] = QString("Booru : ") + QString(booru->getName().c_str()); //Booru
    infos[1] = QString(booru->getName().c_str()) + QString(" URL : ") + QString("<a href=\"") + QString(booru->getShowIndexUrl().c_str()) + QString::number(Image.id) + QString("\">") + QString("Link") + QString("</a>");
    infos[2] = QString(tr("Source URL : ")) + QString("<a href=\"") + QString(Image.source.c_str()) + QString("\">") + QString("Link") + QString("</a>");

    infos[3] = QString(tr("Image ID : ")) + QString::number(Image.id); //ID
    infos[4] = QString(tr("Creator : ")) + QString(Image.author.c_str());//Author
    infos[5] = QString(tr("Created on : ")) + QString(date.c_str());//Date
    infos[6] = QString(tr("Score : ")) + QString::number(Image.score);//Score

    infos[8] = QString(tr("Size : ")) + QString("<a href=\"") + QString(Image.full_url.c_str()) +  QString("\">") + QString::number(Image.width) + "x" + QString::number(Image.height) + QString("</a>");//Picture Size
    infos[9] = QString(tr("File Size : ")) + QString::number(Image.file_size);//File Size

    switch(Image.rating)
    {
    case 'e':
        infos[7] = "Rating : Explicit";
        break;
    case 's':
        infos[7] = "Rating : Safe";
        break;
    case 'q':
        infos[7] = "Rating : Questionnable";
        break;
    }

    for(int i=0;i<10;i++)
    {
        labelInfo[i] = new QLabel(infos[i],this);
        layoutGroupBox->addWidget(labelInfo[i]);
    }

    if(thumbZoneSize.width() < 0)
    {
        thumbZoneSize = labelImage->size();
    }

    labelInfo[0]->setTextFormat(Qt::RichText);
    labelInfo[0]->setTextInteractionFlags(Qt::TextBrowserInteraction);
    labelInfo[0]->setOpenExternalLinks(true);

    labelInfo[1]->setTextFormat(Qt::RichText);
    labelInfo[1]->setTextInteractionFlags(Qt::TextBrowserInteraction);
    labelInfo[1]->setOpenExternalLinks(true);

    labelInfo[8]->setTextFormat(Qt::RichText);
    labelInfo[8]->setTextInteractionFlags(Qt::TextBrowserInteraction);
    labelInfo[8]->setOpenExternalLinks(true);
}

void ViewerTab::downloadPicture()
{
    labelLoading->setText(tr("Downloading Picture (May take some time)"));
    progressBar->setValue(0);
    Image.download_full();
    labelLoading->setText(tr("Completed"));
    progressBar->setValue(100);
}

void ViewerTab::showNextPicture()
{
    if(imageIndex==14)
    {
        imageIndex = 0;

        parentWidget->searchtabs[booru->getIndex()]->on_pushButton_pagePlus_clicked();
        parentWidget->searchtabs[booru->getIndex()]->loadSearch(0);
        parentWidget->searchtabs[booru->getIndex()]->setViewer(imageIndex);
    }
    else
    {
        labelLoading->setText(tr("Caching Picture"));
        imageIndex +=1;
        parentWidget->searchtabs[booru->getIndex()]->setViewer(imageIndex);
    }

    if(imageIndex > 0 || pageIndex >= 1)
    {
        pushButtonSwitchMoins->setDisabled(false);
    }
}

void ViewerTab::showPreviousPicture()
{
    if(imageIndex==0 && pageIndex > 1)
    {
        imageIndex = 14;

        parentWidget->searchtabs[booru->getIndex()]->on_pushButton_pageMoins_clicked();
        parentWidget->searchtabs[booru->getIndex()]->loadSearch(0);
        parentWidget->searchtabs[booru->getIndex()]->setViewer(imageIndex);
    }
    else if(imageIndex>0)
    {
        labelLoading->setText(tr("Caching Picture"));
        imageIndex--;
        parentWidget->searchtabs[booru->getIndex()]->setViewer(imageIndex);
    }


    if(imageIndex==0 && pageIndex == 1)
    {
        pushButtonSwitchMoins->setDisabled(true);
    }
}

void ViewerTab::setKeyboard()
{
    grabKeyboard();
}

bool ViewerTab::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *k = static_cast<QKeyEvent *>(event);
        if(k->key() == Qt::Key_Left)
        {
            showPreviousPicture();
            return QWidget::event(event);
        }
        else if(k->key() == Qt::Key_Right)
        {
            showNextPicture();
            return  QWidget::event(event);
        }
        else
        {
            return false;
        }
    }
    else
    {
    }
    return false;
}

void ViewerTab::startLoadingPicture()
{
    checkIfDownloaded();

    if(!isLoading)
    {
        picture_thread = new QThread;
        worker = new ViewerTabLoadingWorker(this);

        worker->moveToThread(picture_thread);

        connect(picture_thread, SIGNAL(started()), worker, SLOT(process()));
        connect(worker, SIGNAL(finished()), picture_thread, SLOT(quit()));
        connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
        connect(picture_thread, SIGNAL(finished()), picture_thread, SLOT(deleteLater()));

        connect(worker, SIGNAL(finished()), this, SLOT(imageLoaded()));

        picture_thread->start();
    }
}

void ViewerTab::imageLoaded()
{
    progressBar->setValue(100);
    labelLoading->setText(tr("Completed"));

    checkIfDownloaded();
}

void ViewerTab::setLoadingState(bool state)
{
    isLoading = state;
}

void ViewerTab::checkIfDownloaded()
{
    if(fexists(this->Image.getFullPath().c_str()))
    {
        this->pushButtonDownload->setEnabled(false);
        this->pushButtonDownload->setText(tr("Already downloaded"));
    }
    else
    {
        this->pushButtonDownload->setEnabled(true);
        this->pushButtonDownload->setText(tr("Download"));
    }
}


