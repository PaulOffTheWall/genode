/**
 * $Id: edf.h 795 2014-12-11 11:02:52Z klugeflo $
 * @file edf.h
 * @brief EDF Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_EDF_H
#define SCHEDULERS_EDF_H 1

#include <schedulers/ald.h>

namespace tmssim {

  /**
   * @brief Earliest deadline first scheduler
   * @todo implement deadline monitoring, cancel job if deadline miss
   * is pending.
   */
  class EDFScheduler : public ALDScheduler {
    
  public:
    EDFScheduler();
    virtual ~EDFScheduler();

    virtual bool enqueueJob(Job *job);

    /**
     * This function checks whether the first job in the queue, i.e. the
     * job that is executed next, can still keep its deadline.
     * If a deadline miss is pending, the job is immediately cancelled.
     * You may overwrite it in subclasses to implement more sophisticated
     * cancellation policies.
     */
    virtual int schedule(int now, ScheduleStat& scheduleStat);


    virtual const std::string& getId(void) const;

    
    //friend std::ostream& operator << (std::ostream& ost, const EDFScheduler& scheduler);
    
    static const Job* checkEDFSchedule(int now,
				       const list<Job*>& schedule);
    
  protected:
    /*
    virtual int doSchedule(int now, ScheduleStat& scheduleStat);
    virtual Job* doDispatch(int now, DispatchStat& dispatchStat);
    virtual bool fireCancelJob(Job *job);
    */

    /**
      Create a EDF schedule for the current job list
    */
    //void createEDFSchedule(void);

    /**
     * @brief Check a EDF schedule for feasibility.
     * Runs through EDFScheduler::myScheduled an checks whether all jobs can
     * keep their deadlines if the schedule is started at a certain time.
     * @param now current time
     * @return the first job that would miss its deadline, NULL else
     */
    const Job* checkEDFSchedule(int now) const;

  };


  /**
   * @brief Generic allocator function
   */
  static Scheduler* EDFSchedulerAllocator() { return new EDFScheduler; }


  //std::ostream& operator << (std::ostream& ost, const EDFScheduler& scheduler);

} // NS tmssim

#endif /* SCHEDULERS_EDF_H */

