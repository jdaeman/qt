#ifndef SNIFFER_H
#define SNIFFER_H

#include <QThread>
#include <QObject>
#include <nic.h>

class sniffer : public QThread
{
    Q_OBJECT

public:
    sniffer(struct nic * nic, unsigned short proto);
    ~sniffer();

signals:
    void sig_to_sniff_dialog(int type, char * buf, int len);


private:
    void run();
    int create_sniff_sock(int if_index);
    void sniff_sock_close();

    struct nic * sel;
    unsigned short protocol;
    int sock;
};

#endif // SNIFFER_H
