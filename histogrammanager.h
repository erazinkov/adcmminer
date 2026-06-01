#ifndef HISTOGRAMMANAGER_H
#define HISTOGRAMMANAGER_H

#include <memory>
#include <optional>

#include <TH1.h>
#include <TH1D.h>
#include <TH2.h>

class HistogramManager
{
public:
    HistogramManager(std::optional<std::string> outputDirectory = std::nullopt);
    ~HistogramManager();
    std::vector<std::vector<std::shared_ptr<TH1>>> createHistograms(const std::string &histName,
                                                                int nBinsX,
                                                                double xLow,
                                                                double xUp,
                                                                std::vector<int> &idxsGamma,
                                                                std::vector<int> &idxsAlpha) const;
    std::vector<std::shared_ptr<TH1>> createHistograms(const std::string &histName,
                                                                int nBinsX,
                                                                double xLow,
                                                                double xUp,
                                                                std::vector<int> &idxs) const;
    std::vector<std::vector<std::shared_ptr<TH2>>> createHistograms(const std::string &histName,
                                                                int nBinsX,
                                                                double xLow,
                                                                double xUp,
                                                                int nBinsY,
                                                                double yLow,
                                                                double yUp,
                                                                std::vector<int> &idxsGamma,
                                                                std::vector<int> &idxsAlpha) const;
    std::vector<std::shared_ptr<TH2>> createHistograms(const std::string &histName,
                                                                int nBinsX,
                                                                double xLow,
                                                                double xUp,
                                                                int nBinsY,
                                                                double yLow,
                                                                double yUp,
                                                                std::vector<int> &idxs) const;
    void printToPsFile(const std::string &psName,
               std::vector<std::vector<std::shared_ptr<TH1>> > &hists) const;
    void printToPsFile(const std::string &fileName,
                                 std::vector<std::shared_ptr<TH1> > &hists) const;
    void printToPsFile(const std::string &psName,
               std::shared_ptr<TH1> hist) const;
    void saveToRootFile(const std::string &fileName,
                        std::shared_ptr<TH1> hist) const;

    const std::vector<std::vector<TH1D *> > &histsGammaAmp() const;

private:
    static inline constexpr int GAMMA_NUMBER{32};
    static inline constexpr int ALPHA_NUMBER{9};

    static inline constexpr int BINS_TIME{800};
    static inline constexpr int BINS_CHANNEL{400};
    static inline constexpr int BINS_ENERGY{640};

    static inline constexpr double XLOW_TIME{-200.0};
    static inline constexpr double XLOW_CHANNEL{0.0};
    static inline constexpr double XLOW_ENERGY{0.0};

    static inline constexpr double XUP_TIME{200.0};
    static inline constexpr double XUP_CHANNEL{4.0e3};
    static inline constexpr double XUP_ENERGY{8.0e3};

    std::optional<std::string> outputDirectory_;
    std::vector<std::vector<TH1D *>> histsGammaAmp_;
};

#endif // HISTOGRAMMANAGER_H
