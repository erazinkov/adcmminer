#include "calibration.h"

#include <sstream>
#include <future>

#include <TCanvas.h>
#include <TError.h>
#include <TF1.h>

#include "utils.h"
#include "constants.h"
#include "peakfinder.h"
#include "piecewiselinearfunction.h"

Calibration::Calibration()
{
    histogramManager = new HistogramManager(AppConstants::MAX_GAMMA_NUMBER, AppConstants::MAX_ALPHA_NUMBER);
    for (auto ig{0}; ig < AppConstants::MAX_GAMMA_NUMBER; ++ig) {
        for (auto ia{0}; ia < AppConstants::MAX_ALPHA_NUMBER; ++ia) {
            std::pair<uint8_t, uint8_t> p{ig, ia};
            timeCorrections_[p] = 0.0;
        }
    }
}

void Calibration::process()
{
    fillHistsTimeByGammaAlpha(histogramManager->histsTimeByGammaAlpha(), false);
    // TODO
    for (size_t ig{0}; ig < histogramManager->histsTimeByGammaAlpha().size(); ++ig) {
        for (size_t ia{0}; ia <  histogramManager->histsTimeByGammaAlpha().at(ig).size(); ++ia) {
            std::pair<uint8_t, uint8_t> p{ig, ia};
            timeCorrections_[p] = histogramManager->histsTimeByGammaAlpha().at(ig).at(ia)->GetBinCenter(histogramManager->histsTimeByGammaAlpha().at(ig).at(ia)->GetMaximumBin());
        }
    }

    fillHistsTimeByGammaAlpha(histogramManager->histsTimeCorrectedByGammaAlpha(), true);

    fillHistsAmpByGammaAlpha(histogramManager->histsAmpByGammaAlphaSg(), histogramManager->histsAmpByGammaAlphaBg(), histogramManager->histsAmpByGammaAlphaRc());

//    fillHistsAmpByAlpha(histogramManager->histsAmpByGammaAlphaSg(), histogramManager->histsAmpByGammaAlphaBg());
    fillHistsAmpByGamma(histogramManager->histsAmpByGammaAlphaSg(), histogramManager->histsAmpByGammaAlphaBg(), histogramManager->histsAmpByGammaAlphaRc());

    PeakFinder peakFinder(channels_.g.size());
    peakFinder.process(histogramManager->histsAmpByGamma(), histogramManager->histsAmpByGammaRc());
    energyPeaks_ = peakFinder.energyPeaks();

    fillHistsEnergyByGammaAlpha(histogramManager->histsEnergyByGammaAlphaSg(), histogramManager->histsEnergyByGammaAlphaBg());


    fillHistsEnergyByGamma(histogramManager->histsEnergyByGammaAlphaSg(), histogramManager->histsEnergyByGammaAlphaBg());
    fillHistsTimeByAlpha(histogramManager->histsTimeCorrectedByGammaAlpha());
}


void Calibration::fillHistsTimeByGammaAlpha(const std::vector<std::vector<TH1D *> > &hists, bool isCorrected)
{
    std::vector<std::function<void()>> tasks;
    for (size_t i{0}; i < hists.size(); ++i) {
        for (size_t j{0}; j <  hists.at(i).size(); ++j) {
            tasks.push_back([this, &hists, i, j, isCorrected](){
                hists.at(i).at(j)->Reset();
                std::pair<uint8_t, uint8_t> p{*std::next(channels_.g.begin(), i), *std::next(channels_.a.begin(), j)};
                fillHistTime(events_m_[p], hists.at(i).at(j), isCorrected ? timeCorrections_[{i, j}] : 0.0);
            });
        }
    }
    func_async(tasks.begin(), tasks.end());
    tasks.clear();
}

void Calibration::fillHistTime(const std::vector<dec_ev_m_t> &events, TH1 *h, double correction)
{
    for (const auto & item : events) {
        h->Fill(item.tdc - correction);
    }
}

void Calibration::fillHistAmp(const std::vector<dec_ev_m_t> &events, TH1 *h, double minT, double maxT, bool exclude)
{
    for (const auto & item : events) {
        auto t{static_cast<double>(item.tdc)};
        auto a{static_cast<double>(item.amp)};
        if (exclude)
        {
            if (t < minT || maxT < t)
            {
                h->Fill(a);
            }
        }
        else
        {
            if (minT <= t && t <= maxT)
            {
                h->Fill(a);
            }
        }
    }
}

void Calibration::fillHistEnergy(const std::vector<dec_ev_m_t> &events, TH1 *h, double minT, double maxT, bool exclude, TF1 f)
{
    for (const auto & item : events) {
        auto t{static_cast<double>(item.tdc)};
        auto a{static_cast<double>(item.amp)};
        if (exclude)
        {
            if (t < minT || maxT < t)
            {
                h->Fill(f.Eval(a));
            }
        }
        else
        {
            if (minT <= t && t <= maxT)
            {
                h->Fill(f.Eval(a));
            }
        }
    }
}

void Calibration::fillHistsTimeByAlpha(const std::vector<std::vector<TH1D *> > &hists)
{
    for (size_t i{0}; i <  histogramManager->histsTimeCorrectedByAlpha().size(); ++i) {
        histogramManager->histsTimeCorrectedByAlpha()[i]->Reset();
    }
    for (size_t i{0}; i < hists.size(); ++i) {
        for (size_t j{0}; j <  hists.at(i).size(); ++j) {
            histogramManager->histsTimeCorrectedByAlpha()[j]->Add(hists.at(i).at(j));
        }
    }
}

void Calibration::fillHistsAmpByGammaAlpha(const std::vector<std::vector<TH1D *> > &histsSg,
                                           const std::vector<std::vector<TH1D *> > &histsBg,
                                           const std::vector<std::vector<TH1D *> > &histsRc)
{
    std::vector<std::function<void()>> tasks;
    for (size_t i{0}; i < histsSg.size(); ++i) {
        for (size_t j{0}; j <  histsSg.at(i).size(); ++j) {
            tasks.push_back([this, &histsSg, &histsBg, &histsRc, i, j](){
                histsSg.at(i).at(j)->Reset();
                histsBg.at(i).at(j)->Reset();
                histsRc.at(i).at(j)->Reset();
                std::pair<uint8_t, uint8_t> p{*std::next(channels_.g.begin(), i), *std::next(channels_.a.begin(), j)};
                auto minT_sg{timeCorrections_[{i, j}] - 3.0};
                auto maxT_sg{timeCorrections_[{i, j}] + 3.0};
                fillHistAmp(events_m_[p], histsSg.at(i).at(j), minT_sg, maxT_sg, false);
                fillHistAmp(events_m_[p], histsRc.at(i).at(j), minT_sg, maxT_sg, true);
                auto minT_bg{timeCorrections_[{i, j}] - 30.0};
                auto maxT_bg{timeCorrections_[{i, j}] - 20.0};
                fillHistAmp(events_m_[p], histsBg.at(i).at(j), minT_bg, maxT_bg, false);
            });
        }
    }
    func_async(tasks.begin(), tasks.end());
    tasks.clear();
}



void Calibration::fillHistsAmpByAlpha(const std::vector<std::vector<TH1D *>> &histsSg, const std::vector<std::vector<TH1D *> > &histsBg)
{
    for (size_t i{0}; i <  histogramManager->histsAmpByAlpha().size(); ++i) {
        histogramManager->histsAmpByAlpha()[i]->Reset();
    }
    for (size_t i{0}; i < histsSg.size(); ++i) {
        for (size_t j{0}; j <  histsSg.at(i).size(); ++j) {
            histogramManager->histsAmpByAlpha()[j]->Add(histsSg.at(i).at(j));
            histogramManager->histsAmpByAlpha()[j]->Add(histsBg.at(i).at(j), -1.0 * 6.0 / 10.0);
        }
    }
}

void Calibration::fillHistsAmpByGamma(const std::vector<std::vector<TH1D *> > &histsSg,
                                      const std::vector<std::vector<TH1D *> > &histsBg,
                                      const std::vector<std::vector<TH1D *> > &histsRc)
{
    for (size_t i{0}; i <  histogramManager->histsAmpByGamma().size(); ++i) {
        histogramManager->histsAmpByGamma()[i]->Reset();
        histogramManager->histsAmpByGammaRc()[i]->Reset();
    }
    for (size_t i{0}; i < histsSg.size(); ++i) {
        for (size_t j{0}; j <  histsSg.at(i).size(); ++j) {
            histogramManager->histsAmpByGamma()[i]->Add(histsSg.at(i).at(j));
            histogramManager->histsAmpByGamma()[i]->Add(histsBg.at(i).at(j), -1.0 * 6.0 / 10.0);

            histogramManager->histsAmpByGammaRc()[i]->Add(histsRc.at(i).at(j));
        }
    }
}

void Calibration::fillHistsEnergyByGammaAlpha(const std::vector<std::vector<TH1D *> > &histsSg, const std::vector<std::vector<TH1D *> > &histsBg)
{
//    std::cout << "!!!!!!!!!!!!!!!!! " << energyPeaks_.size() << " " << histsSg.size() << std::endl;
//    std::vector<TF1> fs;
//    for (size_t i{0}; i < histsSg.size(); ++i) {
//        if (i < channels_.g.size()) {
//            PiecewiseLinearFunction fObj(energyPeaks_.at(i));
//            TF1 f("f", fObj, 0, 4'000, 0);
//            fs.push_back(f);
//        }
//    }
    std::vector<std::function<void()>> tasks;
    for (size_t i{0}; i < std::min(histsSg.size(), channels_.g.size()); ++i) {
        for (size_t j{0}; j <  std::min(histsSg.at(i).size(), channels_.a.size()); ++j) {
            tasks.push_back([this, &histsSg, &histsBg, i, j](){
                histsSg.at(i).at(j)->Reset();
                histsBg.at(i).at(j)->Reset();
                PiecewiseLinearFunction fObj(energyPeaks_.at(i));
                TF1 f("f", fObj, 0, 4'000, 0);
                std::pair<uint8_t, uint8_t> p{*std::next(channels_.g.begin(), i), *std::next(channels_.a.begin(), j)};
                auto minT_sg{timeCorrections_[{i, j}] - 3.0};
                auto maxT_sg{timeCorrections_[{i, j}] + 3.0};
                fillHistEnergy(events_m_[p], histsSg.at(i).at(j), minT_sg, maxT_sg, false, f);
                auto minT_bg{timeCorrections_[{i, j}] - 30.0};
                auto maxT_bg{timeCorrections_[{i, j}] - 20.0};
                fillHistEnergy(events_m_[p], histsBg.at(i).at(j), minT_bg, maxT_bg, false, f);
            });
        }
    }
    func_async(tasks.begin(), tasks.end());
    tasks.clear();
}

void Calibration::fillHistsEnergyByGamma(const std::vector<std::vector<TH1D *> > &histsSg, const std::vector<std::vector<TH1D *> > &histsBg)
{
    for (size_t i{0}; i <  histogramManager->histsEnergyByGamma().size(); ++i) {
        histogramManager->histsEnergyByGamma()[i]->Reset();
    }
    for (size_t i{0}; i < histsSg.size(); ++i) {
        for (size_t j{0}; j <  histsSg.at(i).size(); ++j) {
            histogramManager->histsEnergyByGamma()[i]->Add(histsSg.at(i).at(j));
            histogramManager->histsEnergyByGamma()[i]->Add(histsBg.at(i).at(j), -1.0 * 6.0 / 10.0);
        }
    }
};

void Calibration::setNewEvents(const std::map<std::pair<uint8_t, uint8_t>, std::vector<dec_ev_m_t> > &newEventsM, const dec_ch_t &channels)
{
    for (const auto& [key, vec] : newEventsM) {
        auto [it, inserted] = events_m_.try_emplace(key, vec);
        if (!inserted) {
            it->second.insert(it->second.end(), vec.begin(), vec.end());
        }
    }
    channels_ = channels;
}


