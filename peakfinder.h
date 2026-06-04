#ifndef PEAKFINDER_H
#define PEAKFINDER_H

#include <TMath.h>
#include <TH1.h>
#include <TF1.h>

#include "energypeak.h"

class PeakFinder
{
public:
    PeakFinder(const u_int8_t gammaNumber);
    void process(std::vector<TH1 *> &histsSg, std::vector<TH1 *> &histsBg);

    const std::vector<std::vector<EnergyPeak> > &energyPeaks() const;

private:
    double calib_;
    double offset_;

    double getE(double ch) { return ch * calib_ + offset_; }
    double getdE(double dCh) { return dCh * calib_; }
    double getCh(double e) { return (e - offset_) / calib_; }
    double getdCh(double de) { return de / calib_; }

    double getFerrum847PosApprox(TH1 *h, double r = 0.12);
    double getFerrum847Pos(TH1 *h);
    double getFerrum1238Pos(TH1 *h);
    double getHydrogenPos(TH1 *h);
    double getCarbonPos(TH1 *h, double appPos);
    double getOxygenPos(TH1 *h, double appPos);
    double getFerrum7631Pos(TH1 *h, double appPos);

    std::vector<std::vector<EnergyPeak>>  energyPeaks_;

};

#endif // PEAKFINDER_H
