#include "sniffer.h"
#include "sniff.h"
#include <QDebug>

Sniffer::Sniffer()
{

}

void Sniffer::run()
{
    unsigned char buf[65536];

    while (true)
    {
        int len = sniff(buf, 65536);

        if (len <= 0)
            continue;

        emit capture(buf, len, NULL);
    }
}

void Sniffer::set_filter(int *filter, int len)
{

}
