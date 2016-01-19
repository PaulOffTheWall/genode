/**
 * $Id: dbpedf.h 715 2014-09-25 09:25:02Z klugeflo $
 * @file dbpedf.h
 * @brief Utility-based EDF using distance from fail state
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_DBPEDF_H
#define SCHEDULERS_DBPEDF_H 1

#include <schedulers/oedfmax.h>

namespace tmssim {

  class DBPEDFScheduler : public OEDFMaxScheduler {
  public:
    DBPEDFScheduler();
    virtual ~DBPEDFScheduler();
    
    virtual const std::string& getId(void) const;
    
  protected:
    virtual double calcValue(int time, const Job *job) const;
    virtual bool isCancelCandidate(double value) const;

  };


  /**
   * @brief Generic allocator function
   */
  static Scheduler* DBPEDFSchedulerAllocator() { return new DBPEDFScheduler; }


} // NS tmssim

#endif // !SCHEDULERS_DBPEDF_H
