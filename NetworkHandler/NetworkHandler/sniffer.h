#ifndef SNIFFER_H
#define SNIFFER_H

#include <QThread>
#include <QObject>

//#include "mutex.h"
//#include <QMutex>

class Sniffer : public QThread
{
    Q_OBJECT

public:
    Sniffer();

signals:
    void capture(unsigned char * buf, int len, void * arg);

private:
    int mtu;

    void run(void);

    void set_filter(int * filter, int len);
};

#endif // SNIFFER_H
