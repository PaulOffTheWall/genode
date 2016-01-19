/**
 * $Id: dvdedf.h 693 2014-09-14 20:32:48Z klugeflo $
 * @file dvdedf.h
 * @brief Dynamic value density scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_DVDEDF_H
#define SCHEDULERS_DVDEDF_H 1

#include <list>

#include <schedulers/oedfmin.h>

namespace tmssim {

  /**
    Implementation of Dynamic Value Density Scheduler.
    See Saud A. Aldarmi and Alan Burns, Dynamic value-density for scheduling
    real-time systems, ECRTS 1999
  */
  class DVDEDFScheduler : public OEDFMinScheduler {
    
  public:
    
    DVDEDFScheduler();
    virtual ~DVDEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(int time, const Job *job) const;
    
  };

  /**
   * @brief Generic allocator function
   */
  static Scheduler* DVDEDFSchedulerAllocator() { return new DVDEDFScheduler; }

} // NS tmssim

#endif /* SCHEDULERS_DVDEDF_H */
