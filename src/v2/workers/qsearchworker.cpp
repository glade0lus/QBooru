#include "qsearchworker.h"

QSearchWorker::QSearchWorker(BooruSearchEngine* engine, QObject *parent) : QObject(parent)
{
    this->engine = engine;
}


void QSearchWorker::process()
{
    std::vector<BooruPicture*> pictures = engine->search();
    emit results(pictures);
    emit finished();
}
