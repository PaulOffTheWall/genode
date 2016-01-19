/**
 * $Id: beedf.h 693 2014-09-14 20:32:48Z klugeflo $
 * @file beedf.h
 * @brief Best effort EDF scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_BEEDF_H
#define SCHEDULERS_BEEDF_H 1

#include <list>

#include <schedulers/oedfmin.h>

namespace tmssim {

  /**
    Implementation of Best-Effort EDF scheduler.
    Based on Jensen et al. RTSS 1985, Locke PhD 1986
  */
  class BEEDFScheduler : public OEDFMinScheduler {
  public:
    
    BEEDFScheduler();
    virtual ~BEEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(int time, const Job *job) const;
    
  };

  /**
   * @brief Generic allocator function
   */
  static Scheduler* BEEDFSchedulerAllocator() { return new BEEDFScheduler; }
  
} // NS tmssim

#endif /* SCHEDULERS_BEEDF_H */

