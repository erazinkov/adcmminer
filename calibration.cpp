#include "calibration.h"

#include <TCanvas.h>
#include <TError.h>

#include <TF1.h>

Calibration::Calibration(const ChannelMap &map, const std::vector<dec_ev_t> &events) : _map(map)
{
    _nGamma = map.numberOfChannelsGamma();
    _nAlpha = map.numberOfChannelsAlpha();

    _timePeaksPos.resize(_nGamma);
    for (auto & item : _timePeaksPos)
    {
        item.resize(_nAlpha, 0.0);
    }
}

Calibration::Calibration(const ChannelMap &map) : _map(map)
{
    _nGamma = map.numberOfChannelsGamma();
    _nAlpha = map.numberOfChannelsAlpha();

    _timePeaksPos.resize(_nGamma);
    for (auto & item : _timePeaksPos)
    {
        item.resize(_nAlpha, 0.0);
    }
    _hists.resize(_nGamma);
    prepareHists("histTime", 400, -100, 100, _hists);
    _histsAmp.resize(_nGamma);
    prepareHists("histAmp", 640, 0, 4e3, _histsAmp);
}



void Calibration::process()
{
    processTime();
    processGammaAmp();
}

const std::vector<std::vector<TH1 *> > &Calibration::hists() const
{
    return _hists;
}

std::vector<dec_ev_t> Calibration::selectedEvents(uint8_t ig, u_int8_t ia)
{
    std::vector<dec_ev_t> selectedEvents{};
    auto it{_newEvents.begin()};

    while ( (it = std::find_if(it, _newEvents.end(), [&ig, &ia](dec_ev_t e){
                               return e.g.index == ig && e.a.index == ia;
})) != _newEvents.end() ) {
        selectedEvents.push_back(*it);
        ++it;
    }
    return selectedEvents;
}

double Calibration::valueTime(const dec_ev_t &event)
{
    return static_cast<double>(event.tdc);
}

double Calibration::valueTimeCorrected(const dec_ev_t &event)
{
    return static_cast<double>(event.tdc) - _timePeaksPos[event.g.index][event.a.index];
}

double Calibration::valueGammaAmp(const dec_ev_t &event)
{
    return static_cast<double>(event.g.amp);
}

void Calibration::calculateTimePeaksPos(std::vector<std::vector<TH1 *> > &hists)
{
    gErrorIgnoreLevel = 3'000;
    for (size_t ig{0}; ig < hists.size(); ++ig)
    {
        for (size_t ia{0}; ia <  hists[ig].size(); ++ia)
        {
            _timePeaksPos[ig][ia] = calculateTimePeakPos(hists[ig][ia]);
        }
    }
    gErrorIgnoreLevel = 0;
}

double Calibration::calculateTimePeakPos(TH1 *hist)
{
    auto timePeakPos{0.0};
    auto h{hist};
    auto binMax{h->GetMaximumBin()};
    auto xMax{h->GetBinCenter(h->GetBin(binMax))};
    auto rcAmp{h->GetBinContent(h->GetXaxis()->FindBin(xMax - 25.0))};
    auto peakAmp{h->GetBinContent(binMax) - rcAmp};
    TF1 *f = new TF1("f", _timePeakFitFunctionObject, xMax - 25.0, xMax + 25.0, 5);
    f->SetParameters(peakAmp, xMax, 5.0, rcAmp, 0.0);
    h->Fit(f, "RQ");
    timePeakPos = f->GetParameter(1);
    delete f;
    f = nullptr;
    return timePeakPos;
}

void Calibration::fillHist(const std::vector<dec_ev_t> &events, TH1 *h, double(Calibration::*f)(const dec_ev_t &event))
{
    for (const auto & item : events)
    {
        auto v{(this->*f)(item)};
        h->Fill(v);
    }
}

void Calibration::drawHistsToFile(const std::string &psName, std::vector<std::vector<TH1 *> > hists)
{
    gErrorIgnoreLevel = 3'000;
    std::unique_ptr<TCanvas> c{new TCanvas("c", "c", 1024, 960)};
    c->Print((psName + '[').c_str());
    for (size_t ig{0}; ig < hists.size(); ++ig)
    {
        c->Divide(3, 3);
        for (size_t ia{0}; ia <  hists[ig].size(); ++ia)
        {
            c->cd(static_cast<int>(ia) + 1);
            hists[ig][ia]->Draw();
        }
        c->Print(psName.c_str());
        c->Clear();
    }
    c->Print((psName + ']').c_str());
    gErrorIgnoreLevel = 0;
}

void Calibration::prepareHists(const std::string &histName, int nBinsX, double xLow, double xUp, std::vector<std::vector<TH1 *> > &hists)
{
    std::stringstream ss;
    for (size_t ig{0}; ig < _nGamma; ++ig)
    {
        for (size_t ia{0}; ia <  _nAlpha; ++ia)
        {
            ss.clear();ss.str("");
            ss << histName << "_" << ig << "_" << ia;
            TH1 *h{new TH1D(ss.str().c_str(), ss.str().c_str(), nBinsX, xLow, xUp)};
            hists[ig].push_back(h);
        }
    }
}

void Calibration::prepareHists(const std::string &histName, int nBinsX, double xLow, double xUp, std::vector<TH1 *> &hists)
{
    std::stringstream ss;
    for (size_t ig{0}; ig < _nGamma; ++ig)
    {
        ss.clear();ss.str("");
        ss << histName << "_" << ig;
        TH1 *h{new TH1D(ss.str().c_str(), ss.str().c_str(), nBinsX, xLow, xUp)};
        hists.push_back(h);
    }
}


void Calibration::clearHists(std::vector<std::vector<TH1 *> > &hists)
{
    for (size_t ig{0}; ig < hists.size(); ++ig)
    {
        for (size_t ia{0}; ia <  hists[ig].size(); ++ia)
        {
            hists[ig][ia]->Reset();
        }
    }
}

void Calibration::deleteHists(std::vector<std::vector<TH1 *> > &hists)
{
    for (size_t ig{0}; ig < hists.size(); ++ig)
    {
        for (size_t ia{0}; ia <  hists[ig].size(); ++ia)
        {
            delete hists[ig][ia];
            hists[ig][ia] = nullptr;
        }
    }
    hists.clear();
}

void Calibration::deleteHists1()
{
    deleteHists(_hists);
}

const std::vector<std::vector<TH1 *> > &Calibration::histsAmp() const
{
    return _histsAmp;
}

void Calibration::setNewEvents(const std::vector<dec_ev_t> &newEvents)
{
    _newEvents = newEvents;
}

void Calibration::fillHistsAsync(const std::vector<std::vector<TH1 *> > &hists, double (Calibration::*f)(const dec_ev_t &))
{
    std::vector<std::future<void>> futures;
    for (size_t ig{0}; ig < hists.size(); ++ig)
    {
        for (size_t ia{0}; ia <  hists[ig].size(); ++ia)
        {
            futures.emplace_back(std::async(std::launch::async, [this, &hists, &f] (u_int8_t g, u_int8_t a) {
                auto sE{selectedEvents(g, a)};
                fillHist(sE, hists[g][a], f);
            }, ig, ia));
        }
    }

    for (size_t i{0}; i < futures.size(); ++i)
    {
        futures[i].get();
    }
}

void Calibration::fillHistsSim(const std::vector<std::vector<TH1 *> > &hists, double (Calibration::*f)(const dec_ev_t &))
{
    for (size_t ig{0}; ig < hists.size(); ++ig)
    {
        for (size_t ia{0}; ia <  hists[ig].size(); ++ia)
        {
            auto sE{selectedEvents(static_cast<u_int8_t>(ig), static_cast<u_int8_t>(ia))};
            fillHist(sE, hists[ig][ia], f);
        }
    }
}

void Calibration::processTime()
{
    std::vector<std::vector<TH1 *>> hists(_nGamma);
    hists.resize(_nGamma);
    prepareHists("histTimeTemp", 400, -100, 100, hists);

    double(Calibration::*f)(const dec_ev_t &event);
    f = &Calibration::valueTime;
    fillHistsAsync(hists, f);

    double(Calibration::*fC)(const dec_ev_t &event);
    fC = &Calibration::valueTimeCorrected;

    for (ulong i{0}; i < _hists.size(); ++i)
    {
        for (ulong j{0}; j < _hists[i].size(); ++j)
        {
            _hists[i][j]->Add(hists[i][j]);
        }
    }
//    const std::string psName{"time.ps"};
//    drawHistsToFile(psName, hists);

    deleteHists(hists);
}

//void Calibration::processTime()
//{
//    std::vector<std::vector<TH1 *>> hists(_nGamma);
//    hists.resize(_nGamma);
//    prepareHists("histTimeTemp", 400, -100, 100, hists);

//    double(Calibration::*f)(const dec_ev_t &event);
//    f = &Calibration::valueTime;
//    const auto start = std::chrono::steady_clock::now();
//    fillHistsAsync(hists, f);

//    const auto stop = std::chrono::steady_clock::now();
//    std::cout << "Time elapsed, ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << std::endl;

//    double(Calibration::*fC)(const dec_ev_t &event);
//    fC = &Calibration::valueTimeCorrected;

//    for (ulong i{0}; i < _hists.size(); ++i)
//    {
//        for (ulong j{0}; j < _hists[i].size(); ++j)
//        {
//            _hists[i][j]->Add(hists[i][j]);
//        }
//    }
////    const std::string psName{"time.ps"};
////    drawHistsToFile(psName, hists);

//    deleteHists(hists);
//}

void Calibration::processGammaAmp()
{
    std::vector<std::vector<TH1 *>> hists(_nGamma);
    prepareHists("histGammaAmp", 640, 0, 4e3, hists);

    double(Calibration::*f)(const dec_ev_t &event);
    f = &Calibration::valueGammaAmp;

    fillHistsAsync(hists, f);

    for (ulong i{0}; i < _histsAmp.size(); ++i)
    {
        for (ulong j{0}; j < _histsAmp[i].size(); ++j)
        {
            _histsAmp[i][j]->Add(hists[i][j]);
        }
    }

//    const std::string psName{"gamma_amp.ps"};
//    drawHistsToFile(psName, hists);

    deleteHists(hists);
}
