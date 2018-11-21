#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtWidgets>
#include <QTranslator>

class QBooruTab;
class QBooruPictureViewer;
class BooruSite;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void loadUI();

public slots:
    void loadBoorus(QVector<BooruSite*> sites);
    void addBooru(BooruSite* site);
    void openViewer();

private:
    void reorderTabs();

    QTabWidget *tab_widget;
    QVector<QBooruTab*> tabs;
    QBooruPictureViewer *viewer;
    QTranslator translator;
};

#endif // MAINWIDGET_H
