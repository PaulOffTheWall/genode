/**
 * $Id: utilisationstatistics.h 703 2014-09-19 12:42:22Z klugeflo $
 * @file utilisationstatistics.h
 * @brief Statistics on task sets' processor utilisation
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_UTILISATIONSTATISTICS_H
#define MKEVAL_UTILISATIONSTATISTICS_H 1

#include <list>
#include <vector>

namespace tmssim {

  /**
   * @brief Collect and evaluates task sets' processor utilisation statistics
   */
  class UtilisationStatistics {
  public:
    UtilisationStatistics();
    ~UtilisationStatistics();

    /**
     * @brief Add a new utilisation value
     */
    void addUtilisation(double utilisation);

    void evaluate();

    double getSize() const;
    double getMean() const;
    double getSigma() const;
    double getMedian() const;
    double getMin() const;
    double getMax() const;

    double getMedianIntervalLower(int percent) const;
    double getMedianIntervalUpper(int percent) const;

  private:
    std::list<double> uList;
    std::vector<double> data;

    bool evaluated;

    double mean;
    double var;
    double sigma;
    double median;

  };


} // NS tmssim

#endif // !MKEVAL_UTILISATIONSTATISTICS_H
