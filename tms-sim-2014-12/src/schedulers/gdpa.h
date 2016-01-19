/**
 * $Id: gdpa.h 694 2014-09-15 15:22:39Z klugeflo $
 * @file gdpa.h
 * @brief Guaranteed Dynamic Priority Assigment Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULER_GDPA_H
#define SCHEDULER_GDPA_H 1

#include <schedulers/edf.h>


namespace tmssim {

  /**
   * @brief GDPA Scheduler.
   *
   * This class implements GDPA from Cho et al., "Guaranteed Dynamic
   * Priority Assignment Schemes", ETRI Journal Vol. 32 No. 3, June 2010
   */
  class GDPAScheduler : public EDFScheduler {
  public:
    GDPAScheduler();
    virtual ~GDPAScheduler();

    virtual bool enqueueJob(Job *job);

    virtual bool hasPendingJobs(void) const;

    /**
     * @brief This function calculates the actual GDPA schedule.
     *
     * This method implements algorithm 1 from Cho et al. 2010
     */
    virtual int schedule(int now, ScheduleStat& scheduleStat);


    virtual const std::string& getId(void) const;

  protected:
    virtual void jobFinished(Job *job);


  private:
    std::list<Job*> readyQueue;
    bool readyQueueChanged;
  };


  /**
   * @brief Generic allocator function
   */
  static Scheduler* GDPASchedulerAllocator() { return new GDPAScheduler; }

} // NS tmssim

#endif // !SCHEDULER_GDPA_H
