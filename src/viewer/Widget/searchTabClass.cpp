#include "fonctions_all.h"
#include "widget.h"
#include "searchTabClass.h"
#include "imageTabClass.h"

#include "../classBooruSite.h"

SearchTab::SearchTab(Widget *parent, BooruSite* site) : QWidget(parent)
{
    conf_file = parent->getConfigFile();

    picture_number = conf_file->getPictureNumber();
    picture_rows = conf_file->getPictureRow();
    picture_columns = conf_file->getPictureColumns();

    booru = site;

    booru_search_engine = new BooruSearchEngine(booru, this);
    booru_search_engine->setImageCount(picture_number);


    recherche = 0;
    int i=0;
    int j=0;
    int k=0;

    for(i=0;i<64;i++)
    {
        isLoading[i] = false;
    }

    parentWidget = parent;

    installEventFilter(this);

    signalMapper = new QSignalMapper(this);
    buttonMapper = new QSignalMapper(this);

    /*Déclaration des objects*/
    layoutMain = new QVBoxLayout;

        layoutSearch = new QHBoxLayout;
        layoutTags = new QHBoxLayout;

            /*Searchbar*/
            loginButton = new QPushButton(tr("Login"),this);
            searchButton = new QPushButton(tr("Refresh"),this);
            dumpButton = new QPushButton(tr("Dump Page"),this);
            lineEditTags = new QLineEdit;
            lineEditTags->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
            lineEditTags->setMaximumHeight(21);

            if(booru->getSiteTypeInt() != GELBOORU_TYPE)
            {
                //Don't show the button
                loginButton->hide();
            }

            //Load the cookies
            cookie = new CookieJar(QString(this->booru->getName().c_str()),this);
            cookie->load();

            if(!cookie->isEmpty())
            {
                //Indicates it on the button
                loginButton->setText(tr("Login OK"));
            }


            /*Autocompletion*/
            if(booru->isTagListToLoad())
            {
                booru->loadTagList();
                if(booru->isTagListLoaded())
                {
                    QStringList wordList;
                    for(int i=0;i<booru->getTagList()->size();i++)
                    {
                        if(booru->isTagListLoaded())
                        {
                            wordList << QString(booru->getTagList()->getTag(i)->getName().c_str());
                        }
                    }
                    QCompleter *completer = new QCompleter(wordList, this);
                    completer->setCaseSensitivity(Qt::CaseInsensitive);
                    lineEditTags->setCompleter(completer);
                }
            }

            /*Search Status*/
            layoutSearchStatus = new QVBoxLayout;
                progressBarSearch = new QProgressBar;
                progressBarSearch->setMaximumHeight(21);
                labelSearchStatus = new QLabel(tr("Waiting for search"));
                labelSearchStatus->setMaximumHeight(21);

            layoutSearchRating = new QHBoxLayout;

            searchRating = new QComboBox;
            searchRating->addItem(tr("All"));
            searchRating->addItem("Safe");
            searchRating->addItem("Questionnable");
            searchRating->addItem("Explicit");
            searchRating->setMaximumHeight(21);

        widgetImageTabs = new QWidget(this);
        layoutImageTabs = new QGridLayout;
            for(i=0;i<picture_number;i++)
            { 
                images[i].setBooru(this->booru);
                imageTabs[i] = new ImageTab(this, booru->getSiteTypeInt());
                buttonMapper->setMapping(imageTabs[i]->imageThumbnail, i);
                connect(imageTabs[i]->imageThumbnail, SIGNAL(clicked()), buttonMapper , SLOT(map()));

                thread_pool_loading[i] = new QThread;
                loading_worker[i] = new SearchTabLoadingWorker();
            }

        layoutSearchPage = new QHBoxLayout;
            pushButtonPageMoins = new QPushButton("<--");
            lineEditPageSet = new QLineEdit("1");
            lineEditPageSet->setStyleSheet("qproperty-alignment: AlignCenter;");
            pushButtonPagePlus = new QPushButton("-->");

    /*Assignation des layout*/

        /*SearchStatus*/
        layoutSearchStatus->addWidget(progressBarSearch);
        layoutSearchStatus->addWidget(labelSearchStatus);

    /*Search elements*/
    QVBoxLayout *verticalLayoutSearch = new QVBoxLayout;
    layoutSearchRating->addWidget(loginButton);
    layoutSearchRating->addWidget(dumpButton);
    layoutSearchRating->addWidget(searchRating);
    layoutSearchRating->addWidget(searchButton);
    layoutSearchRating->addWidget(lineEditTags);
    verticalLayoutSearch->addLayout(layoutSearchRating);
    verticalLayoutSearch->addLayout(layoutTags);
    layoutSearch->addLayout(verticalLayoutSearch,3);
    layoutSearch->addLayout(layoutSearchStatus,1);

    /*SearchPage*/
    layoutSearchPage->addStretch(1);
    layoutSearchPage->addWidget(pushButtonPageMoins);
    layoutSearchPage->addWidget(lineEditPageSet);
    layoutSearchPage->addWidget(pushButtonPagePlus);
    layoutSearchPage->addStretch(1);

    /*ImageTabs*/
    for(j=0;j<picture_rows;j++)
    {
        for(i=0;i<picture_columns;i++)
        {
           layoutImageTabs->addWidget(imageTabs[k], j, i);
           layoutImageTabs->setAlignment(imageTabs[k],Qt::AlignCenter);
           k++;
        }
    }
    layoutImageTabs->setVerticalSpacing(0);
    layoutImageTabs->setHorizontalSpacing(0);

    widgetImageTabs->setLayout(layoutImageTabs);

    signalMapper->setMapping(searchButton, 0);
    signalMapper->setMapping(lineEditTags, 1);
    signalMapper->setMapping(lineEditPageSet, 2);
    signalMapper->setMapping(pushButtonPageMoins, 3);
    signalMapper->setMapping(pushButtonPagePlus, 4);

    connect(searchButton, SIGNAL(clicked()), signalMapper , SLOT(map()));
    connect(lineEditTags, SIGNAL(returnPressed()),signalMapper,SLOT(map()));
    connect(lineEditPageSet, SIGNAL(returnPressed()),signalMapper,SLOT(map()));
    connect(pushButtonPageMoins, SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(pushButtonPagePlus, SIGNAL(clicked()),signalMapper,SLOT(map()));

    connect(dumpButton,SIGNAL(clicked()),this,SLOT(startDumpingPicture()));

    connect(loginButton, SIGNAL(clicked()), this , SLOT(login()));

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(searchActionToggled(int)));
    connect(buttonMapper, SIGNAL(mapped(int)), this, SLOT(setViewer(int)));


    /*Main Layout*/
    layoutMain->addLayout(layoutSearch);
    layoutMain->addWidget(widgetImageTabs);
    layoutMain->addLayout(layoutSearchPage);

    setLayout(layoutMain);

    initialisationMiniatures();


    pushButtonPagePlus->setDisabled(true);

    for(i=0;i<picture_number;i++)
    {
        imageTabs[i]->setDisabled(true);
        isAffiched[i] = false;
    }
    checkPageButtonStatus();

    searchRating->setCurrentIndex(conf_file->getPreferredRating());
}

SearchTab::~SearchTab()
{
    /*
    int i;
    for(i=0;i<picture_number;i++)
    {
        delete imageTabs[i];
    }
    clearLayoutSecond(layoutMain);
    */
}

void SearchTab::login()
{
    /*Login test*/

    QString user = QInputDialog::getText(this, "UserID", tr("Please input your Username (needed to log in to Gelbooru)"), QLineEdit::Normal, QString());
    QString pass = QInputDialog::getText(this, "Pass", tr("Please input your Password (needed to log in to Gelbooru)"), QLineEdit::Normal, QString());

    QList<QNetworkCookie> cookies = getLoginCookie("http://gelbooru.com/index.php?page=account&s=login&code=00",user,pass);

    cookie->setCookiesFromUrl(cookies,QString(this->booru->getBaseUrl().c_str()));

    if(cookie->isEmpty())
    {
        QMessageBox::warning(this,tr("Warning"),tr("Login cookies could not be retrieved. Please check your username/password"));
    }
    else
    {
        qDebug() << "Loaded" << QString(this->booru->getName().c_str()) << "cookie" << cookie->getAllCookies();
        loginButton->setText(tr("Login OK"));
    }
}

void SearchTab::setLoadingState(int i, bool state)
{
    isLoading[i] = state;
}

void SearchTab::initialisationMiniatures()
{
    int i = 0;
    QString pathFile, uploader;

    pathFile = QString(CONF_PATH)+QString("derpibooruDefault.png");

    if(recherche)
    {
        uploader = tr("Loading picture");
    }
    else
    {
        uploader = tr("Waiting for search");
    }

    for(i=0;i<picture_number;i++)
    {
        imageTabs[i]->setDisabled(false);
        imageTabs[i]->loadPicture(pathFile, uploader);
    }
}


void SearchTab::loadTagViewer(QString tagString)
{
    lineEditTags->setText(tagString);
    lineEditPageSet->setText("1");
    loadSearch(1);
}

void SearchTab::loadSearch(int refreshTags)
{
    recherche = 1;
    QString tags;
    QString pathFile =  QString(CONF_PATH)+QString("derpibooruDefault.png");
    progress = 0;
    int state = 0;

    if(refreshTags == 1)
    {
        tags = lineEditTags->text();
        booru_search_engine->setRating(searchRating->currentIndex());
    }
    else
    {
        tags = tagsSearched;
    }

    initialisationMiniatures();

    updateSearchStatus(progress, tr("Loading search page"));

    /*Tags*/

    booru_search_engine->search(tags.toStdString(),lineEditPageSet->text().toInt(),this->cookie);

    /*Chargement*/

    if(state == 0)
    {
        loaded_pictures = 0;

        progress +=20;//Progress = 20
        updateSearchStatus(progress, tr("Loading pictures"));

        for(int i=0;i<picture_number;i++)
        {
            images[i].loadData(i);
            progress += 20/picture_number ;
        }

        //Progress = 40
        updateSearchStatus(progress, tr("Loading thumbnails"));

        loaded_pictures = 0;

        for(int i=0;i<picture_number;i++)
        {
            if(images[i].id != 0)
            {
                imageTabs[i]->setDisabled(false);
                isAffiched[i] = true;
                startLoadingPicture(i);
            }
            else
            {
                imageTabs[i]->loadPicture(pathFile, "...");
                imageTabs[i]->setDisabled(true);
                isAffiched[i] = false;
            }
        }
        outputInfo(L_INFO,tr("Search completed").toStdString());
    }
    else
    {
        labelSearchStatus->setText(QString(tr("Error loading search (Error ")) + QString::number(state) + QString(")"));
    }
    tagsSearched = tags;
}

void SearchTab::searchActionToggled(int sender)
{
    //0 = Search Button
    //1 = lineEdit Tags
    //2 = lineEdit Page Set
    //3 = pushButton Page Moins
    //4 = pushButton Page Plus
    int refresh = 0;

    switch(sender)
    {
    case 0:
    case 1:
        refresh = 1;
        break;
    case 2:
        refresh = 0;
        break;
    case 3:
        on_pushButton_pageMoins_clicked();
        refresh = 0;
        break;
    case 4:
        on_pushButton_pagePlus_clicked();
        refresh = 0;
        break;
    default:
        refresh = 0;
        break;
    }

    if(refresh == 1)
    {
        lineEditPageSet->setText("1");
    }

    loadSearch(refresh);
}

void SearchTab::on_pushButton_pagePlus_clicked()
{
    int pageNumber;
    QString pageText;
    QString tags = tagsSearched;

    pageText = lineEditPageSet->text();

    pageNumber = pageText.toInt();
    pageNumber++;

    lineEditPageSet->setText(QString::number(pageNumber));
    checkPageButtonStatus();
}

void SearchTab::on_pushButton_pageMoins_clicked()
{
    int pageNumber;
    QString pageText;

    pageText = lineEditPageSet->text();

    pageNumber = pageText.toInt();

    if(pageNumber > 1)
    {
        pageNumber--;
        lineEditPageSet->setText(QString::number(pageNumber));
    }
    checkPageButtonStatus();
}

void SearchTab::setViewer(int sender)
{
    parentWidget->viewerTab->loadPicture(booru->getSiteTypeInt(), booru->getIndex(), sender, lineEditPageSet->text().toInt(), booru);
}

void SearchTab::checkPageButtonStatus()
{
    int i;

    if(lineEditPageSet->text().toInt() == 1)
    {
        pushButtonPageMoins->setDisabled(true);
    }
    else
    {
        pushButtonPageMoins->setDisabled(false);
    }

    for(i=0;i<picture_number;i++)
    {
        if(isAffiched)
        {
            pushButtonPagePlus->setDisabled(false);
        }
        else
        {
            pushButtonPagePlus->setDisabled(true);
        }
    }
}

void SearchTab::setKeyboard()
{
    //grabKeyboard();
}

bool SearchTab::eventFilter(QObject *object, QEvent *event)
{
    if (object != lineEditTags && object != lineEditPageSet && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *k = static_cast<QKeyEvent *>(event);
        if(k->key() == Qt::Key_Left)
        {
            on_pushButton_pageMoins_clicked();
            loadSearch(0);
            return QWidget::event(event);
        }
        else if(k->key() == Qt::Key_Right)
        {
            on_pushButton_pagePlus_clicked();
            loadSearch(0);
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

void SearchTab::newSearch()
{
    loadSearch(1);
}

void SearchTab::startLoadingPicture(int i)
{
    if(isLoading[i] == false)
    {
        images[i].setBooru(booru);

        thread_pool_loading[i] = new QThread;
        loading_worker[i] = new SearchTabLoadingWorker(this,i);

        loading_worker[i]->moveToThread(thread_pool_loading[i]);

        connect(thread_pool_loading[i], SIGNAL(started()), loading_worker[i], SLOT(process()));
        connect(loading_worker[i], SIGNAL(finished()), thread_pool_loading[i], SLOT(quit()));
        connect(loading_worker[i], SIGNAL(finished()), loading_worker[i], SLOT(deleteLater()));
        connect(loading_worker[i], SIGNAL(finished(int)), this, SLOT(image_loaded(int)));
        connect(thread_pool_loading[i], SIGNAL(finished()), thread_pool_loading[i], SLOT(deleteLater()));

        thread_pool_loading[i]->start();
    }
}

void SearchTab::startDumpingPicture()
{
    dump_progress = 0;

    dumpButton->setDisabled(true);
    dumpButton->setText("Dumping");

    for(int i=0;i<conf_file->getPictureNumber();i++)
    {
        QThread *thread = new QThread(this);
        ImageDownloadWorker *worker = new ImageDownloadWorker(&images[i]);

        worker->moveToThread(thread);

        connect(thread, SIGNAL(started()), worker, SLOT(process()));
        connect(worker, SIGNAL(finished()), this, SLOT(updateDumping()));
        connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();
    }
}

void SearchTab::updateDumping()
{
    dump_progress++;
    dumpButton->setText(tr("Dumped ") + QString::number(dump_progress) + QString("/") + QString::number(conf_file->getPictureNumber()));

    if(dump_progress >= conf_file->getPictureNumber())
    {
        dumpButton->setEnabled(true);
        dumpButton->setText(tr("Dump page"));
    }
}

void SearchTab::image_loaded(int index)
{
    loaded_pictures++;
    progress += 4;
    if(progressBarSearch){progressBarSearch->setValue(progress);parentWidget->viewerTab->progressBar->setValue(progress);} //Sometimes the progressBar has a SIGSEGV

    labelSearchStatus->setText(QString(tr("Loaded thumbnails ")) + QString::number(loaded_pictures) + QString("/") + QString::number(picture_number));
    parentWidget->viewerTab->labelLoading->setText(QString(tr("Loaded thumbnails ")) + QString::number(loaded_pictures) + QString("/") + QString::number(picture_number));
    //delete loading_worker[i];
    //delete thread_pool_loading[i];
}

void SearchTab::updateSearchStatus(int progress, QString text)
{
    progressBarSearch->setValue(progress);
    parentWidget->viewerTab->progressBar->setValue(progress);
    labelSearchStatus->setText(text);
    parentWidget->viewerTab->labelLoading->setText(text);
}

ConfigFile *SearchTab::getConfigFile()
{
    return this->conf_file;
}

QWidget* SearchTab::getImageTabsWidget()
{
    return this->widgetImageTabs;
}
