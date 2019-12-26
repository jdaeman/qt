#include "sniffer.h"
#include "sniff.h"
#include <QDebug>

Sniffer::Sniffer()
{

}

void Sniffer::run()
{
    unsigned char buf[1514]; //Normal MTU + Ethernet header size
    unsigned char * cpy;
    int len;

    while (true)
    {
        memset(buf, 0, sizeof(buf));
        len = sniff(buf, 1514);

        if (len <= 0)
            continue;

        //Synchronization with sniffdialog
        cpy = new unsigned char[len];
        memcpy(cpy, buf, len);

        emit capture(cpy, len, NULL);
    }
}

void Sniffer::set_filter(int *filter, int len)
{

}
