/**
 * $Id: job.h 788 2014-12-09 12:59:15Z klugeflo $
 * @file job.h
 * @brief Definition of Job class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_JOB_H
#define CORE_JOB_H 1

#include <string>

namespace tmssim {

  class Task;

  /**
    @brief Representation of a job that must be executed.
    Jobs represent the units of work that are executed by a Scheduler.
  */
  class Job {
    
  public:
    /**
      @param task owner task
      @param jid job id (usually job number)
      @param _activationTime activation time
      @param _executionTime execution time span
      @param _absDeadline absolute Deadline
      @param _priority static priority (only for static priority scheduling)
    */
    Job(Task* task, unsigned int jid, int _activationTime, int _executionTime, int _absDeadline, int _priority);
    //Job(const Job& rhs);
    
    int getActivationTime(void) const; ///< get activation time
    int getExecutionTime(void) const; ///< get execution time
    int getAbsDeadline(void) const; ///< get critial time
    int getRemainingExecutionTime(void) const; ///< get remaining execution time
    Task* getTask(void) const; ///< get owner task
    int getPriority(); ///< get (static) priority
    int getLatestStartTime(void) const; ///< get latest start time

    void updateLatestStartTime(void);
    
    bool execStep(void); ///< execute one step; @return true if remaining ET is zero
    void preempt(); ///< notification from scheduler - job is preempted
    void setPriority(int p); ///< set static priority
    int getPreemptions() const; ///< get number of  preemptions
    
    /**
      @brief calculate possible benefit if execution start at startTime
      @param startTime
      @return utility that can be achieved
      @see #tmssim::Task::getPossibleExecValue
    */
    double getPossibleExecValue(int startTime) const;
    
    /**
      @brief calculate possible history-cognisant utility of task if execution
      start at startTime.
      @param startTime
      @return HCU the task can achieve
      @see #tmssim::Task::getPossibleHistoryValue
    */
    double getPossibleHistoryValue(int startTime) const;

    /**
     * @brief calculate possible history-cognisant utility of task
     * if job is cancelled
     * @return HCU the task can achieve
     * @see #tmssim::Task::getPossibleHistoryValue
     */
    double getPossibleFailHistoryValue() const;
    
    std::string getIdString(void) const; ///< get job identifier for verbose output
    
  protected:
    Task* myTask; ///< owner task
    unsigned int jobId; ///< usually job number
    int activationTime; ///< arrival time
    int executionTime; ///< execution time
    int absDeadline; ///< critical time
    int priority; ///< priority
    int etRemain; ///< remaining execution time
    int preemptions; ///< count dispatch/schedule preemptions
    int latestStartTime; ///< latest start time
    
  public:
    friend std::ostream& operator << (std::ostream& ost, const Job& job);
  };


} // NS tmssim

#endif // !CORE_JOB_H
