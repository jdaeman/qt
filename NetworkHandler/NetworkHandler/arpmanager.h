#ifndef ARPMANAGER_H
#define ARPMANAGER_H

#include <QThread>

class ArpManager : public QThread
{
public:
    ArpManager();

signals:


private:
    void run();
};

#endif // ARPMANAGER_H
