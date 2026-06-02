#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "adcm_df.h"

#include <TH1.h>
#include <TMath.h>

#include <histogrammanager.h>

#include <vector>
#include <algorithm>
#include <cstdint>
#include <map>

class Calibration
{
public:
    Calibration();

    void process();


    const std::vector<std::vector<TH1 *> > &hists() const;
    void deleteHists(std::vector<std::vector<TH1 *>> &hists);
    void deleteHists1();


    const std::vector<std::vector<TH1 *> > &histsAmp() const;

    void setNewEvents(const std::vector<dec_ev_t> &newEvents, const dec_ch_t &channels);
    void setNewEventsM(const std::map<std::pair<uint8_t, uint8_t>, std::vector<dec_ev_m_t>> &newEventsM, const dec_ch_t &channels);

    const std::vector<TH1 *> &histsAmpPoGamma() const;

    HistogramManager histogramManager_;
private:

    std::map<std::pair<uint8_t, uint8_t>, std::vector<dec_ev_m_t>> events_m_;


    std::vector<dec_ev_t> _newEvents;
    dec_ch_t _channels;
    std::vector<std::vector<TH1 *>> _hists;
    std::vector<std::vector<TH1 *>> _histsAmp;
    std::vector<TH1 *> _histsAmpPoGamma;

    std::vector<dec_ev_t> selectedEvents(uint8_t ig, u_int8_t ia);
    void fillHist(const std::vector<dec_ev_m_t> &events, TH1 *h, double(Calibration::*f)(const dec_ev_t &event));
    void fillHistsAsync(const std::vector<std::vector<TH1D *>> &hists, double(Calibration::*f)(const dec_ev_t &event));
    void fillHistsSim(const std::vector<std::vector<TH1 *>> &hists, double(Calibration::*f)(const dec_ev_t &event));
    void drawHistsToFile(const std::string &psName, std::vector<std::vector<TH1 *>> hists);
    void prepareHists(const std::string &histName,
                      int nBinsX,
                      double xLow,
                      double xUp,
                      std::vector<std::vector<TH1 *>> &hists);
    void prepareHists(const std::string &histName,
                      int nBinsX,
                      double xLow,
                      double xUp,
                      std::vector<TH1 *> &hists);
    void clearHists(std::vector<std::vector<TH1 *>> &hists);

    void processTime();
    void processGammaAmp();

    double valueTime(const dec_ev_t &event);
    double valueTimeCorrected(const dec_ev_t &event);
    double valueGammaAmp(const dec_ev_t &event);

    std::vector<std::vector<double>> _timePeaksPos;
    unsigned long _nGamma;
    unsigned long _nAlpha;

    void calculateTimePeaksPos(std::vector<std::vector<TH1 *> > &hists);
    double calculateTimePeakPos(TH1 *hist);
    void calculateAmpPeakPos(TH1 *hist);

    class TimePeakFitFunctionObject
    {
    public:
        TimePeakFitFunctionObject(){}

        double operator() (double *x, double *par) {
           double arg{0};
           if (par[2] != 0.0)
           {
               arg = ( x[0] - par[1] ) / par[2];
           }
           double fitval{par[0] * TMath::Exp(-0.5 * arg * arg) + par[3] + par[4] * x[0]};
           return fitval;
       }
    };
    TimePeakFitFunctionObject _timePeakFitFunctionObject;
    class AmpPeakFitFunctionObject
    {
    public:
        AmpPeakFitFunctionObject(){}

        double operator() (double *x, double *par) {
           double arg{0};
           if (par[2] != 0.0)
           {
               arg = ( x[0] - par[1] ) / par[2];
           }
           double fitval{par[0] * TMath::Exp(-0.5 * arg * arg) + par[3] + par[4] * x[0]};
           return fitval;
       }
    };
    AmpPeakFitFunctionObject _ampPeakFitFunctionObject;
};

#endif // CALIBRATION_H
