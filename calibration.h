#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "adcm_df.h"


#include <histogrammanager.h>
#include "timepeaksfinder.h"
#include "energypeak.h"

class Calibration
{
public:
    Calibration();

    void process();
    void setNewEvents(const std::map<std::pair<uint8_t, uint8_t>, std::vector<dec_ev_m_t>> &newEventsM, const dec_ch_t &channels);
    HistogramManager *histogramManager;
private:

    std::map<std::pair<uint8_t, uint8_t>, std::vector<dec_ev_m_t>> events_m_;
    std::map<std::pair<uint8_t, uint8_t>, double> timeCorrections_;
    dec_ch_t channels_;
    std::unique_ptr<TimePeaksFinder> timePeaksFinder_; // TODO
    std::vector<std::vector<EnergyPeak>>  energyPeaks_;

    void fillHistTime(const std::vector<dec_ev_m_t> &events, TH1 *h, double correction);
    void fillHistAmp(const std::vector<dec_ev_m_t> &events, TH1 *h, double minT, double maxT, bool exclude);
    void fillHistEnergy(const std::vector<dec_ev_m_t> &events, TH1 *h, double minT, double maxT, bool exclude, TF1 f);

    void fillHistsTimeByGammaAlpha(const std::vector<std::vector<TH1D *>> &hists, bool isCorrected = false);
    void fillHistsTimeByAlpha(const std::vector<std::vector<TH1D *>> &hists);

    void fillHistsAmpByGammaAlpha(const std::vector<std::vector<TH1D *> > &histsSg,
                                  const std::vector<std::vector<TH1D *> > &histsBg,
                                  const std::vector<std::vector<TH1D *> > &histsRc);
    void fillHistsAmpByAlpha(const std::vector<std::vector<TH1D *>> &histsSg, const std::vector<std::vector<TH1D *> > &histsBg);
    void fillHistsAmpByGamma(const std::vector<std::vector<TH1D *> > &histsSg,
                             const std::vector<std::vector<TH1D *> > &histsBg,
                             const std::vector<std::vector<TH1D *> > &histsRc);


    void fillHistsEnergyByGammaAlpha(const std::vector<std::vector<TH1D *> > &histsSg,
                                  const std::vector<std::vector<TH1D *> > &histsBg);

    void fillHistsEnergyByGamma(const std::vector<std::vector<TH1D *> > &histsSg,
                                  const std::vector<std::vector<TH1D *> > &histsBg);


    std::vector<std::vector<double>> _timePeaksPos;

};

#endif // CALIBRATION_H
