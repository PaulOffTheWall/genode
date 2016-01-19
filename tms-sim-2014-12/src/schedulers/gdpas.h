/**
 * $Id: gdpas.h 694 2014-09-15 15:22:39Z klugeflo $
 * @file gdpas.h
 * @brief Simplified Guaranteed Dynamic Priority Assigment Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_GDPAS_H
#define SCHEDULERS_GDPAS_H 1

#include <core/scheduler.h>

namespace tmssim {

  /**
   * @brief GDPA-S Scheduler.
   *
   * This class implements GDPA-S from Cho et al., "Guaranteed Dynamic
   * Priority Assignment Schemes", ETRI Journal Vol. 32 No. 3, June 2010
   */
  class GDPASScheduler : public Scheduler {
  public:
    GDPASScheduler();
    virtual ~GDPASScheduler();

    virtual bool enqueueJob(Job *job);
    virtual Job* removeJob(Job *job);

    virtual bool hasPendingJobs(void) const;

    /**
     * @brief This function calculates the actual GDPA-S schedule.
     *
     * This method implements algorithm 2 from Cho et al. 2010
     */
    virtual int schedule(int now, ScheduleStat& scheduleStat);

    virtual Job* dispatch(int now, DispatchStat& dispatchStat);

    virtual const std::string& getId(void) const;

  protected:
    virtual void jobFinished(Job *job);


  private:
    void feasibilityCheck(int now);
    void removeJobFromList(std::list<Job*>& jList, Job* job);
    std::list<Job*> readyList;
    std::list<Job*> sdfList;
    std::list<Job*> edfList;
    bool readyListChanged;
    bool dispatchListsChanged;
    //bool queuesChanged;
    //bool edfFeasible;
    Job* currentJob;
  };


  /**
   * @brief Generic allocator function
   */
  static Scheduler* GDPASSchedulerAllocator() { return new GDPASScheduler; }

} // NS tmssim

#endif // !SCHEDULERS_GDPAS_H
