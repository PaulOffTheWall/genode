/**
 * $Id: oedfmax.h 795 2014-12-11 11:02:52Z klugeflo $
 * @file oedfmax.h
 * @brief Overload EDF scheduler, maximum value jobs are removed
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_OEDFMAX_H
#define SCHEDULERS_OEDFMAX_H 1

#include <list>

#include <schedulers/oedf.h>

namespace tmssim {

  /**
    This scheduler can handle overloads in an EDF scheduler. It removes jobs
    that have maximum value under some metric. This metric must be implemented
    in OEDFMaxScheduler::calcValue
  */
  class OEDFMaxScheduler : public OEDFScheduler {
    
  public:
    
    OEDFMaxScheduler();
    virtual ~OEDFMaxScheduler();
    //virtual int schedule(int now, ScheduleStat& scheduleStat);
    
  protected:
    //virtual double calcValue(int time, const Job *job) const = 0;
    //virtual bool isCancelCandidate(double value) const { return true; }

    virtual double getComparisonNeutral() const;
    virtual bool compare(double current, double candidate) const;

  };
  
} // NS tmssim

#endif /* SCHEDULERS_OEDFMAX_H */

