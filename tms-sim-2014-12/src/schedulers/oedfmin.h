/**
 * $Id: oedfmin.h 795 2014-12-11 11:02:52Z klugeflo $
 * @file oedfmin.h
 * @brief EDF with cancellation, minimum value cancel policy
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_OEDFMIN_H
#define SCHEDULERS_OEDFMIN_H 1

#include <list>

#include <schedulers/oedf.h>

namespace tmssim {
  
  /**
    This scheduler can handle overloads in an EDF scheduler. It removes jobs
    that have minimum value under some metric. This metric must be implemented
    in OEDFMinScheduler::calcValue
  */
  class OEDFMinScheduler : public OEDFScheduler {
    
  public:
    
    OEDFMinScheduler();
    virtual ~OEDFMinScheduler();
    //virtual int schedule(int now, ScheduleStat& scheduleStat);

  protected:
    //virtual double calcValue(int time, const Job *job) const = 0;
    //virtual bool isCancelCandidate(double value) const { return true; }

    virtual double getComparisonNeutral() const;
    virtual bool compare(double current, double candidate) const;
    
  };
  
} // NS tmssim

#endif /* SCHEDULERS_OEDFMIN_H */
