/**
 * $Id: deadlinemonitor.h 659 2014-08-29 14:01:32Z klugeflo $
 * @file deadlinemonitor.h
 * @brief Deadline Monitor for use in scheduler implementations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_DEADLINEMONITOR_H
#define CORE_DEADLINEMONITOR_H 1

#include <core/scobjects.h>
#include <list>

namespace tmssim {

  /**
   * @brief Deadline Monitor for use in scheduler implementations.
   * 
   * Pending deadline misses are identified as soon as the latest possible
   * start time for job has elapsed.
   */
  class DeadlineMonitor {
  public:
    DeadlineMonitor();
    ~DeadlineMonitor();

    /**
     * @brief Add job to the monitoring list
     */
    void addJob(const Job* job);

    /**
     * @brief Check if any job will miss its deadline.
     *
     * This function checks, whether any job in the list will miss its
     * deadline at the current time. If there is such a job, it will
     * be removed from the list and be returned to the caller. To remove
     * all jobs that will miss their deadlines from the list, call this
     * method until it returns NULL.
     * @param now the current time
     * @return NULL, if there is no job that will miss its deadline, else
     * a job will be returned.
     */
    const Job* check(int now);

    /**
     * @brief notify the monitor that a job was executed for some time,
     * but is not yet finished.
     *
     * This function adjusts the job's latest starting time and, if
     * necessary, reorders the monitoring list.
     */
    const Job* jobExecuted(const Job *job);

    /**
     * @brief notify that a job has finished execution and can be
     * removed from the monitoring list.
     */
    const Job* jobFinished(const Job* job);

  private:
    /**
     * Contains current jobs ordered increasingly by their latest
     * starting times.
     */
    std::list<const Job*> jobs;
  };

} // NS tmssim

#endif /* !CORE_DEADLINEMONITOR_H */
