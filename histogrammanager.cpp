#include "histogrammanager.h"

#include <sstream>

#include <iostream>

#include <TCanvas.h>
#include <TError.h>
#include <TFile.h>

HistogramManager::HistogramManager(const int &gammaNumber, const int &alphaNumber, std::optional<std::string> outputDirectory) : gammaNumber_{gammaNumber}, alphaNumber_{alphaNumber}, outputDirectory_{outputDirectory}
{
    histsAmpByGammaAlphaSg_.resize(gammaNumber_);
    for (auto ig{0}; ig < gammaNumber_; ig++) {
        histsAmpByGammaAlphaSg_[ig].resize(alphaNumber_);
        for (auto ia{0}; ia < alphaNumber_; ia++) {
            std::string name{Form("hist_amp_by_gamma_sg_%d_alpha_%d", ig, ia)};
            std::string title{Form("hist_amp_by_gamma_sg_%d_alpha_%d", ig, ia)};
            TH1D *h = new TH1D(name.c_str(), title.c_str(), BINS_CHANNEL, XLOW_CHANNEL, XUP_CHANNEL);
            histsAmpByGammaAlphaSg_[ig][ia] = h;
        }
    }
    histsAmpByGammaAlphaBg_.resize(gammaNumber_);
    for (auto ig{0}; ig < gammaNumber_; ig++) {
        histsAmpByGammaAlphaBg_[ig].resize(alphaNumber_);
        for (auto ia{0}; ia < alphaNumber_; ia++) {
            std::string name{Form("hist_amp_by_gamma_bg_%d_alpha_%d", ig, ia)};
            std::string title{Form("hist_amp_by_gamma_bg_%d_alpha_%d", ig, ia)};
            TH1D *h = new TH1D(name.c_str(), title.c_str(), BINS_CHANNEL, XLOW_CHANNEL, XUP_CHANNEL);
            histsAmpByGammaAlphaBg_[ig][ia] = h;
        }
    }
    histsTimeByGammaAlpha_.resize(gammaNumber_);
    for (auto ig{0}; ig < gammaNumber_; ig++) {
        histsTimeByGammaAlpha_[ig].resize(alphaNumber_);
        for (auto ia{0}; ia < alphaNumber_; ia++) {
            std::string name{Form("hist_time_by_gamma_%d_alpha_%d", ig, ia)};
            std::string title{Form("hist_time_by_gamma_%d_alpha_%d", ig, ia)};
            TH1D *h = new TH1D(name.c_str(), title.c_str(), BINS_TIME, XLOW_TIME, XUP_TIME);
            histsTimeByGammaAlpha_[ig][ia] = h;
        }
    }
    histsAmpByAlpha_.resize(alphaNumber_);
    for (auto ia{0}; ia < alphaNumber_; ia++) {
        std::string name{Form("hist_amp_by_alpha_%d", ia)};
        std::string title{Form("hist_amp_by_alpha_%d", ia)};
        TH1D *h = new TH1D(name.c_str(), title.c_str(), BINS_CHANNEL, XLOW_CHANNEL, XUP_CHANNEL);
        histsAmpByAlpha_[ia] = h;
    }
    histsTimeCorrectedByGammaAlpha_.resize(gammaNumber_);
    for (auto ig{0}; ig < gammaNumber_; ig++) {
        histsTimeCorrectedByGammaAlpha_[ig].resize(alphaNumber_);
        for (auto ia{0}; ia < alphaNumber_; ia++) {
            std::string name{Form("hist_time_corrected_by_gamma_%d_alpha_%d", ig, ia)};
            std::string title{Form("hist_time_corrected_by_gamma_%d_alpha_%d", ig, ia)};
            TH1D *h = new TH1D(name.c_str(), title.c_str(), BINS_TIME, XLOW_TIME, XUP_TIME);
            histsTimeCorrectedByGammaAlpha_[ig][ia] = h;
        }
    }
    histsTimeCorrectedByAlpha_.resize(alphaNumber_);
    for (auto ia{0}; ia < alphaNumber_; ia++) {
        std::string name{Form("hist_time_corrected_by_alpha_%d", ia)};
        std::string title{Form("hist_time_corrected_by_alpha_%d", ia)};
        TH1D *h = new TH1D(name.c_str(), title.c_str(), BINS_TIME, XLOW_TIME, XUP_TIME);
        histsTimeCorrectedByAlpha_[ia] = h;
    }
}

HistogramManager::~HistogramManager()
{
    for (auto ig{0}; ig < gammaNumber_; ig++) {
        for (auto ia{0}; ia < alphaNumber_; ia++) {
            delete histsAmpByGammaAlphaSg_[ig][ia];
            histsAmpByGammaAlphaSg_[ig][ia] = nullptr;
            delete histsTimeByGammaAlpha_[ig][ia];
            histsTimeByGammaAlpha_[ig][ia] = nullptr;
        }
    }
    for (auto ia{0}; ia < alphaNumber_; ia++) {
            delete histsAmpByAlpha_[ia];
            histsAmpByAlpha_[ia] = nullptr;
            delete histsTimeCorrectedByAlpha_[ia];
            histsTimeCorrectedByAlpha_[ia] = nullptr;
    }
}

std::vector<std::vector<std::shared_ptr<TH1> > > HistogramManager::createHistograms(const std::string &histName,
                                                                                    int nBinsX,
                                                                                    double xLow,
                                                                                    double xUp,
                                                                                    std::vector<int> &idxsGamma,
                                                                                    std::vector<int> &idxsAlpha) const
{
    std::vector<std::vector<std::shared_ptr<TH1>>> hists;
    hists.resize(idxsGamma.size());
    std::stringstream ss;
    for (size_t i{0}; i < idxsGamma.size(); ++i)
    {
        for (size_t j{0}; j <  idxsAlpha.size(); ++j)
        {
            ss.clear();ss.str("");
            ss << histName << "_" << idxsGamma.at(i) << "_" << idxsAlpha.at(j);
            auto h{std::make_shared<TH1D>(ss.str().c_str(), ss.str().c_str(), nBinsX, xLow, xUp)};
            h->Sumw2();
            hists.at(i).push_back(h);
        }
    }
    return hists;
}

std::vector<std::shared_ptr<TH1> > HistogramManager::createHistograms(const std::string &histName,
                                                                      int nBinsX,
                                                                      double xLow,
                                                                      double xUp,
                                                                      std::vector<int> &idxs) const
{
    std::vector<std::shared_ptr<TH1>> hists;
    std::stringstream ss;
    for (size_t i{0}; i < idxs.size(); ++i)
    {
        ss.clear();ss.str("");
        ss << histName << "_" << idxs.at(i);
        auto h{std::make_shared<TH1D>(ss.str().c_str(), ss.str().c_str(), nBinsX, xLow, xUp)};
        h->Sumw2();
        hists.push_back(h);
    }
    return hists;
}

std::vector<std::vector<std::shared_ptr<TH2> > > HistogramManager::createHistograms(const std::string &histName,
                                                                                    int nBinsX,
                                                                                    double xLow,
                                                                                    double xUp,
                                                                                    int nBinsY,
                                                                                    double yLow,
                                                                                    double yUp,
                                                                                    std::vector<int> &idxsGamma,
                                                                                    std::vector<int> &idxsAlpha) const
{
    std::vector<std::vector<std::shared_ptr<TH2>>> hists;
    hists.resize(idxsGamma.size());
    std::stringstream ss;
    for (size_t i{0}; i < idxsGamma.size(); ++i)
    {
        for (size_t j{0}; j <  idxsAlpha.size(); ++j)
        {
            ss.clear();ss.str("");
            ss << histName << "_" << idxsGamma.at(i) << "_" << idxsAlpha.at(j);
            auto h{std::make_shared<TH2D>(ss.str().c_str(), ss.str().c_str(), nBinsX, xLow, xUp, nBinsY, yLow, yUp)};
            h->Sumw2();
            hists.at(i).push_back(h);
        }
    }
    return hists;
}

std::vector<std::shared_ptr<TH2> > HistogramManager::createHistograms(const std::string &histName,
                                                                      int nBinsX,
                                                                      double xLow,
                                                                      double xUp,
                                                                      int nBinsY,
                                                                      double yLow,
                                                                      double yUp,
                                                                      std::vector<int> &idxs) const
{
    std::vector<std::shared_ptr<TH2>> hists;
    std::stringstream ss;
    for (size_t i{0}; i < idxs.size(); ++i)
    {
        ss.clear();ss.str("");
        ss << histName << "_" << idxs.at(i);
        auto h{std::make_shared<TH2D>(ss.str().c_str(), ss.str().c_str(), nBinsX, xLow, xUp, nBinsY, yLow, yUp)};
        h->Sumw2();
        hists.push_back(h);
    }
    return hists;
}

void HistogramManager::printToPsFile(const std::string &fileName,
                             std::vector<std::shared_ptr<TH1> > &hists) const
{
    const std::string psName{(outputDirectory_.has_value() ? (outputDirectory_.value() + "/") : " ") + fileName + ".ps"};
    gErrorIgnoreLevel = 3'000;
    std::unique_ptr<TCanvas> c{new TCanvas("c", "c", 1024, 960)};
    c.get()->Print((psName + '[').c_str());
    for (size_t ig{0}; ig < hists.size(); ++ig)
    {
        hists.at(ig).get()->Draw();
        auto listOfFunctions{hists.at(ig).get()->GetListOfFunctions()};
        for (auto *item : *listOfFunctions)
        {
            item->Draw("SAME");
        }
        c.get()->Print(psName.c_str());
    }
    c.get()->Print((psName + ']').c_str());
    gErrorIgnoreLevel = 0;
}

void HistogramManager::printToPsFile(const std::string &fileName,
                             std::vector<std::vector<std::shared_ptr<TH1> > > &hists) const
{
    const std::string psName{(outputDirectory_.has_value() ? (outputDirectory_.value() + "/") : " ") + fileName + ".ps"};
    gErrorIgnoreLevel = 3'000;
    std::unique_ptr<TCanvas> c{new TCanvas("c", "c", 1024, 960)};
    c.get()->Print((psName + '[').c_str());
    for (size_t ig{0}; ig < hists.size(); ++ig)
    {
        auto cd{static_cast<int>(std::ceil(std::sqrt(hists.at(ig).size())))};
        c.get()->Divide(cd, cd);
        for (size_t ia{0}; ia <  hists.at(ig).size(); ++ia)
        {
            c.get()->cd(static_cast<int>(ia) + 1);
            hists.at(ig).at(ia).get()->Draw();
            auto listOfFunctions{hists.at(ig).at(ia).get()->GetListOfFunctions()};
            for (auto *item : *listOfFunctions)
            {
                item->Draw("SAME");
            }
        }
        c.get()->Print(psName.c_str());
        c.get()->Clear();
    }
    c.get()->Print((psName + ']').c_str());
    c.get()->Delete();
    gErrorIgnoreLevel = 0;
}

//void HistogramManager::printToPsFile(const std::string &fileName,
//                             std::vector<std::vector<std::shared_ptr<TH1> > > &hists) const
//{
//    const std::string psName{(_outputDirectory.has_value() ? (_outputDirectory.value() + "/") : " ") + fileName + ".ps"};
//    gErrorIgnoreLevel = 3'000;
//    std::unique_ptr<TCanvas> c{new TCanvas("c", "c", 1024, 960)};
//    c.get()->Print((psName + '[').c_str());
//    for (size_t ig{0}; ig < hists.size(); ++ig)
//    {
//        auto cd{static_cast<int>(std::ceil(std::sqrt(hists.at(ig).size())))};
//        c.get()->Divide(cd, cd);
//        for (size_t ia{0}; ia <  hists.at(ig).size(); ++ia)
//        {
//            c.get()->cd(static_cast<int>(ia) + 1);
//            hists.at(ig).at(ia).get()->Draw();
//            auto listOfFunctions{hists.at(ig).at(ia).get()->GetListOfFunctions()};
//            for (auto *item : *listOfFunctions)
//            {
//                item->Draw("SAME");
//            }
//        }
//        c.get()->Print(psName.c_str());
//        c.get()->Clear();
//    }
//    c.get()->Print((psName + ']').c_str());
//    gErrorIgnoreLevel = 0;
//}

void HistogramManager::printToPsFile(const std::string &fileName, std::shared_ptr<TH1> hist) const
{
    const std::string psName{(outputDirectory_.has_value() ? (outputDirectory_.value() + "/") : " ") + fileName + ".ps"};
    gErrorIgnoreLevel = 3'000;
    std::unique_ptr<TCanvas> c{new TCanvas("c", "c", 1024, 960)};
    c.get()->Print((psName + '[').c_str());
    hist.get()->Draw();
    auto listOfFunctions{hist->GetListOfFunctions()};
    for (auto *item : *listOfFunctions)
    {
//        item->Draw("SAME");
    }
    c.get()->Print(psName.c_str());
    c.get()->Print((psName + ']').c_str());
    gErrorIgnoreLevel = 0;
}

void HistogramManager::saveToRootFile(const std::string &fileName, std::shared_ptr<TH1> hist) const
{
    const std::string rootName{(outputDirectory_.has_value() ? (outputDirectory_.value() + "/") : " ") + fileName + ".root"};
    std::unique_ptr<TFile> file{TFile::Open(rootName.c_str(), "RECREATE")};
    if (file.get()->IsOpen())
    {
        hist.get()->Write(hist->GetName(), TObject::kOverwrite);
    }
}

const std::vector<std::vector<TH1D *> > &HistogramManager::histsAmpByGammaAlpha() const
{
    return histsAmpByGammaAlphaSg_;
}

const std::vector<std::vector<TH1D *> > &HistogramManager::histsTimeByGammaAlpha() const
{
    return histsTimeByGammaAlpha_;
}

const std::vector<TH1D *> &HistogramManager::histsAmpByAlpha() const
{
    return histsAmpByAlpha_;
}

const std::vector<std::vector<TH1D *> > &HistogramManager::histsTimeCorrectedByGammaAlpha() const
{
    return histsTimeCorrectedByGammaAlpha_;
}

const std::vector<TH1D *> &HistogramManager::histsTimeCorrectedByAlpha() const
{
    return histsTimeCorrectedByAlpha_;
}

const std::vector<std::vector<TH1D *> > &HistogramManager::histsAmpByGammaAlphaSg() const
{
    return histsAmpByGammaAlphaSg_;
}

const std::vector<std::vector<TH1D *> > &HistogramManager::histsAmpByGammaAlphaBg() const
{
    return histsAmpByGammaAlphaBg_;
}
