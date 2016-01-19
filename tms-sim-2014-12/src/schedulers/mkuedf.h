/**
 * $Id: mkuedf.h 693 2014-09-14 20:32:48Z klugeflo $
 * @file mkuedf.h
 * @brief HCUF-based (m,k)-firm real-time scheduling
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_MKUEDF_H
#define SCHEDULERS_MKUEDF_H 1

#include <schedulers/oedfmax.h>

namespace tmssim {

  class MKUEDFScheduler : public OEDFMaxScheduler {
  public:
    MKUEDFScheduler();
    virtual ~MKUEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(int time, const Job *job) const;
    virtual bool isCancelCandidate(double value) const;

  };


  /**
   * @brief Generic allocator function
   */
  static Scheduler* MKUEDFSchedulerAllocator() { return new MKUEDFScheduler; }

} // NS tmssim

#endif // !SCHEDULERS_MKUEDF_H
