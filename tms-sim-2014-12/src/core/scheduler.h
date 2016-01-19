/**
 * $Id: scheduler.h 693 2014-09-14 20:32:48Z klugeflo $
 * @file scheduler.h
 * @brief Definition of the abstract scheduler class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_SCHEDULER_H
#define CORE_SCHEDULER_H 1

#include <string>
#include <vector>

#include <core/scobjects.h>
//#include <utils/logger.h>

namespace tmssim {

  /// Use this macro to check Job* return values
#define SC_ERR(e) ((Job *)(e))
  

  /**
   * Scheduling statistics.
   * @todo make list contain const Job*
   */
  struct ScheduleStat {
    ScheduleStat();
    ~ScheduleStat();
    
    std::list<Job*> cancelled;
  };
  
  
  /**
    Dispatch statistics
  */
  struct DispatchStat {
    DispatchStat();
    
    const Job* executed;
    const Job* finished;
    bool dlMiss;
    bool idle;
  };
  

  /**
   * @brief Interface for the implementation of Schedulers
   * Inherit from from this class to implement your own schedulers
   */
  class Scheduler {
  public: // errors
    // Schedule errors
    static const int ESC_SCED_SRCH = -1; ///< No job for removal found
    // Dispatch errors
    static const int ESC_DISP_TIME = -1; ///< Time step too big
    static const int ESC_DISP_COMP = -2; ///< Job completion failed
    static const int ESC_DISP_LIST = -3; ///< Job list changed since last schedule call
    static const int ESC_DISP_STAT = -4; ///< Invalid dispatch statistics

  public:
    //Scheduler(/*Logger* _logger = noLogPtr*/);// : logger(_logger) {}

    /**
     * @brief Virtual destructor for subclassing
     * Make sure in your implementation to check whether the scheduler
     * still contains pending jobs. If so, take care to clean up
     * their memory correctly!
     */
    virtual ~Scheduler() = 0;

    /**
     * @brief Enqueue a new job for execution
     *
     * Any job that is enqueued must be returned to the simulation later
     * either by being cancelled via the scheduleStat parameter of
     * the #schedule method, or by finishing execution as a return
     * value of the #dispatch method.
     * @param job a new Job
     */
    virtual bool enqueueJob(Job *job) = 0;

    /**
     * @brief Remove an active job that was previously enqueued
     * @return A pointer to the removed job, or NULL if no job was found
     */
    virtual Job* removeJob(Job *job) = 0;

    
    /**
     * @brief Create the schedule
     * Implementations may decide to leave this function empty and
     * perform the relevant work already during the enqueueJob method.
     * @param now Point in time, succeeding calls to this functions must have
     * increasing times
     * @param[out] scheduleStat Container for statistics. If the scheduler
     * decides to cancel some jobs, they will be stored in this container.
     * @return 0 on success, some error code else
     */
    virtual int schedule(int now, ScheduleStat& scheduleStat) = 0;
    
    /**
     * Dispatch one job from the schedule for execution
     * @param now Point in time, succeeding calls to this functions must have
     * increasing times
     * @param[out] dispatchStat Container for statistics. If a job finished
     * execution, it is also written to the dispatchStat struct.
     * @return finished job, NULL (running/idling), or ESC_DISP error code
     */
    virtual Job* dispatch(int now, DispatchStat& dispatchStat) = 0;
    
    /**
     * @brief Check if there are enqueued jobs
     */
    virtual bool hasPendingJobs(void) const = 0;
    
    /**
     * @brief Get some readable ID from a scheduler
     */
    virtual const std::string& getId(void) const = 0;

    /**
     * @brief output the current schedule.
     * This method is mostly for debugging purposes.
     */
    virtual void printSchedule() const {}

  protected:
    /**
     * @brief Logging/tracing of execution
     */
    //Logger* logger;
    
  };

  inline Scheduler::~Scheduler() { }
  
} // NS tmssim

#endif /* CORE_SCHEDULER_H */
