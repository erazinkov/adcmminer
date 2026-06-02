#include "decoder.h"

#include <iostream>
#include <bits/stdc++.h>

Decoder::Decoder(const std::string &fileName)
    : fileName_{fileName}
{
}

std::vector<dec_ev_t> &Decoder::events()
{
    return events_;
}

void Decoder::process()
{
    ifs_.open(fileName_, std::ios::in | std::ios::binary);
    if (!ifs_.is_open())
    {
        std::clog << "Can't open file" << std::endl;
        return;
    }

    events_.clear();
    events_1_.clear();

    stor_packet_hdr_t hdr;
    stor_ev_hdr_t ev;
    adcm_cmap_t cmap;
    adcm_counters_t counters;

    while (ifs_)
    {
        ifs_ >> hdr;

        if (hdr.id == STOR_ID_CMAP && hdr.size > sizeof(stor_packet_hdr_t))
        {
            ifs_ >> cmap;
            continue;
        }
        if (hdr.id == STOR_ID_EVNT && hdr.size > sizeof(stor_packet_hdr_t))
        {
            ifs_ >> ev;
            switch (ev.np)
            {
                case 1:
                {
                    dec_ev_1_t event;
                    std::unique_ptr<stor_puls_t> d{new stor_puls_t()};
                    ifs_ >> *d.get();
                    event.d.index = d.get()->ch;
                    event.d.amp = d.get()->a;
                    event.tdc = d.get()->t;
                    event.ts = ev.ts;
                    events_1_.push_back(event);
                    channels_.d.insert(d.get()->ch);
                    break;
                }
                case 2:
                {
                    std::unique_ptr<stor_puls_t> g{new stor_puls_t()};
                    std::unique_ptr<stor_puls_t> a{new stor_puls_t()};
                    ifs_ >> *g.get() >> *a.get();
                    dec_ev_t event;
                    event.g.index = g.get()->ch;
                    event.g.amp = g.get()->a;
                    event.a.index = a.get()->ch;
                    event.a.amp = g.get()->a;
                    event.tdc = g.get()->t - a.get()->t;
                    event.ts = ev.ts;
                    events_m_[{g.get()->ch, a.get()->ch}].push_back(event);
//                    events_.push_back(event);
                    channels_.g.insert(g.get()->ch);
                    channels_.a.insert(a.get()->ch);
                    break;
                }
                default:
                {
                    hdr.size -= sizeof(stor_packet_hdr_t);
                    hdr.size -= sizeof(stor_ev_hdr_t);
                    ifs_.ignore(hdr.size);
                    break;
                }
            }
            continue;
        }
        if (hdr.id == STOR_ID_CNTR && hdr.size > sizeof(stor_packet_hdr_t))
        {
            ifs_ >> counters;
            if (!counters.n)
            {
                continue;
            }

            if (counters_.rawhits.empty())
            {
                counters_.rawhits.resize(counters.n);
            }

            for (size_t i{0}; i < counters.n; ++i)
            {
                counters_.rawhits.at(i) += counters.rawhits.at(i);
            }
            counters_.time += counters.time;
//            hdr.size -= sizeof(stor_packet_hdr_t);
//            ifs_.ignore(hdr.size);
            continue;
        }
        ifs_.seekg(1 - static_cast<long long>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    }
    ifs_.close();
}

std::vector<dec_ev_t> Decoder::processA()
{
    ifs_.open(fileName_, std::ios::in | std::ios::binary);
    if (!ifs_.is_open())
    {
        std::clog << "Can't open file" << std::endl;
        return {};
    }

    std::vector<dec_ev_t> events;

    stor_packet_hdr_t hdr;
    stor_ev_hdr_t ev;
    adcm_cmap_t cmap;
    adcm_counters_t counters;


    while (ifs_)
    {
        ifs_ >> hdr;

        if (hdr.id == STOR_ID_CMAP && hdr.size > sizeof(stor_packet_hdr_t))
        {
            ifs_ >> cmap;
            continue;
        }
        if (hdr.id == STOR_ID_EVNT && hdr.size > sizeof(stor_packet_hdr_t))
        {
            ifs_ >> ev;
            if (ev.np != 2)
            {
                hdr.size -= sizeof(stor_packet_hdr_t);
                hdr.size -= sizeof(stor_ev_hdr_t);
                ifs_.ignore(hdr.size);
                continue;
            }
            std::unique_ptr<stor_puls_t> g{new stor_puls_t()};
            std::unique_ptr<stor_puls_t> a{new stor_puls_t()};
            ifs_ >> *g.get() >> *a.get();
            dec_ev_t event;
            event.g.index = g.get()->ch;
            event.g.amp = g.get()->a;
            event.a.index = a.get()->ch;
            event.a.amp = g.get()->a;
            event.tdc = g.get()->t - a.get()->t;
            event.ts = ev.ts;
            events_.push_back(event);

            continue;
        }
        if (hdr.id == STOR_ID_CNTR && hdr.size > sizeof(stor_packet_hdr_t))
        {
            hdr.size -= sizeof(stor_packet_hdr_t);
            ifs_.ignore(hdr.size);
            continue;
        }
        ifs_.seekg(1 - static_cast<long long>(sizeof(stor_packet_hdr_t)), std::ios_base::cur);
    }
    ifs_.close();

    return events;
}

const std::vector<dec_ev_1_t> &Decoder::events_1() const
{
    return events_1_;
}

const dec_cnt_t &Decoder::counters() const
{
    return counters_;
}

const dec_ch_t &Decoder::channels() const
{
    return channels_;
}

const std::map<std::pair<uint8_t, uint8_t>, std::vector<dec_ev_t> > &Decoder::events_m() const
{
    return events_m_;
}


