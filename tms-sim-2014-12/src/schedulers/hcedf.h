/**
 * $Id: hcedf.h 693 2014-09-14 20:32:48Z klugeflo $
 * @file hcedf.h
 * @brief History-cognisant overload EDF scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_HCEDF_H
#define SCHEDULERS_HCEDF_H 1

#include <list>

#include <schedulers/oedfmax.h>

namespace tmssim {

  /**
    History-cognisant overload EDF scheduler.
    Removes jobs with maximum value of task utility from schedule if overload
    is detected.
    See Kluge/Gerdes/Haas/Ungerer @ RTNS 2013.
  */
  class HCEDFScheduler : public OEDFMaxScheduler {
    
  public:
    
    HCEDFScheduler();
    virtual ~HCEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(int time, const Job *job) const;
    
  };


  /**
   * @brief Generic allocator function
   */
  static Scheduler* HCEDFSchedulerAllocator() { return new HCEDFScheduler; }

} // NS tmssim

#endif /* SCHEDULERS_HCEDF_H */

