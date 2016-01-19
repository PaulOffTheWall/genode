/**
 * $Id: fpp.h 806 2014-12-12 11:14:03Z klugeflo $
 * @file fpp.h
 * @brief Fixed Priority Preemptive Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 * @todo adjust stdint.h include to cstdint as soon as C++ 2011 is stable
 */

#include <schedulers/ald.h>
#include <core/deadlinemonitor.h>

//#include <cstdint>
/**
 */
#include <stdint.h>
#include <limits.h>

#ifndef SCHEDULERS_FPP_H
#define SCHEDULERS_FPP_H 1

namespace tmssim {

  /**
   * @brief Fixed Priority Preemptive Scheduling
   * @todo implement deadline monitoring, cancel job if deadline miss
   * is pending.
   */
  class FPPScheduler : public ALDScheduler {
  public:
    static const unsigned int MAX_PRIORITY = 0;
    static const unsigned int MIN_PRIORITY = UINT_MAX;

  public:
    FPPScheduler();
    virtual ~FPPScheduler();
    
    /**
     * @brief Enqueue a job for priority-ordered execution.
     * Jobs are executed in ascending order of their priority values.
     * FPPScheduler::MAX_PRIORITY (0) represents the most important
     * jobs, jobs with priority FPPScheduler::MIN_PRIORITY (UINT_MAX)
     * are executed last.
     */
    virtual bool enqueueJob(Job *job);

    /**
     * This function does nothing. You may overwrite it in subclasses
     * to e.g. check whether a schedule is feasible and, if not, make
     * it feasible.
     */
    virtual int schedule(int now, ScheduleStat& scheduleStat);

    virtual Job* dispatch(int now, DispatchStat& dispatchStat);


    virtual const std::string& getId(void) const;


    
    
  protected:
    /*
     * @brief Check a FPP schedule for feasibility.
     * Runs through FPPScheduler::mySchedule an checks whether all jobs can
     * keep their deadlines if the schedule is started at a certain time.
     * @param now current time
     * @return the first job that would miss its deadline, NULL else
     */
    //const Job* checkFPPSchedule(int now) const;
    DeadlineMonitor dlmon;

  }; // class FPPScheduler


  /**
   * @brief Generic allocator function
   */
  static Scheduler* FPPSchedulerAllocator() { return new FPPScheduler; }

} // NS tmssim

#endif /* !SCHEDULERS_FPP_H */
