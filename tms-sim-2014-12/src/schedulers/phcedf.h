/**
 * $Id: phcedf.h 693 2014-09-14 20:32:48Z klugeflo $
 * @file phcedf.h
 * @brief History-cognisant overload EDF scheduler (weight with remaining
 * execution time)
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_PHCEDF_H
#define SCHEDULERS_PHCEDF_H 1

#include <list>

#include <schedulers/oedfmax.h>

namespace tmssim {

  /**
    History-cognisant overload EDF scheduler with dynamic value density.
    Removes jobs with maximum value of task utility from schedule if overload
    is detected. The task utility is weighted by the remaining execution
    time of the job.
    See Kluge/Gerdes/Haas/Ungerer @ RTNS 2013.
  */
  class PHCEDFScheduler : public OEDFMaxScheduler {
    
  public:
    
    PHCEDFScheduler();
    virtual ~PHCEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(int time, const Job *job) const;
    
  };


  /**
   * @brief Generic allocator function
   */
  static Scheduler* PHCEDFSchedulerAllocator() { return new PHCEDFScheduler; }

} // NS tmssim

#endif /* SCHEDULERS_PHCEDF_H */

