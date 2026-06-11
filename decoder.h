#ifndef DECODER_H
#define DECODER_H

#include "adcm_df.h"
#include "channelmap.h"

#include <map>

class Decoder
{
public:
    Decoder(const std::string &);
    std::vector<dec_ev_t> & events();
    void process();
    std::vector<dec_ev_t> processA();
    const std::vector<dec_ev_1_t> &events_1() const;

    const dec_cnt_t &counters() const;
    const dec_ch_t &channels() const;

    const std::map<std::pair<uint8_t, uint8_t>, std::vector<dec_ev_m_t> > &events_m() const;

private:
    std::string fileName_;
    std::ifstream ifs_;
//    ChannelMap pre_;
    std::vector<dec_ev_t> events_;
    dec_cnt_t counters_;
    std::vector<dec_ev_1_t> events_1_;

    std::map<std::pair<uint8_t, uint8_t>, std::vector<dec_ev_m_t>> events_m_;

    dec_ch_t channels_;
};


#endif // DECODER_H
