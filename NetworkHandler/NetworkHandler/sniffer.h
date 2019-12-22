#ifndef SNIFFER_H
#define SNIFFER_H

#include <QThread>
#include <QObject>
#include "nic.h"

class Sniffer : public QThread
{
    Q_OBJECT

public:
    Sniffer();

signals:
    void capture(unsigned char * buf, int len, void * arg);

private:
    void run(void);

    void set_filter(int * filter, int len);
};

#endif // SNIFFER_H
