#ifndef SPOOFER_H
#define SPOOFER_H

#include <QThread>
#include <nic.h>

class spoofer : public QThread
{
public:
    spoofer(struct nic * nic, unsigned int vicitm);
    ~spoofer();

private:
    void run();

    int sock;
    struct nic * nic;
    unsigned int victim;
};

#endif // SPOOFER_H
