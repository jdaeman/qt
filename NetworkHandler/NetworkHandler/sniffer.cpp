#include "sniffer.h"
#include "sniff.h"
#include <QDebug>

Sniffer::Sniffer()
{

}

void Sniffer::run()
{
    unsigned char buf[65536];
    int len;

    while (true)
    {
        len = sniff(buf, 65536);
        if (len > 0)
            emit capture(buf, len, NULL);
        else
            break;
    }
}
