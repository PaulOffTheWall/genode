/**
 * $Id: oedf.h 795 2014-12-11 11:02:52Z klugeflo $
 * @file oedf.h
 * @brief EDF scheduler with optional execution to resolve overloads
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef SCHEDULERS_OEDF_H
#define SCHEDULERS_OEDF_H 1

#include <schedulers/edf.h>

namespace tmssim {

  class OEDFScheduler : public EDFScheduler {
  public:
    
    OEDFScheduler();
    virtual ~OEDFScheduler();
    virtual int schedule(int now, ScheduleStat& scheduleStat);

    // concrete subclasses need to re-implement this method!
    virtual const std::string& getId(void) const = 0;

  protected:
    /**
     * The comparison procedure needs an initialisation value, which
     * must be provided by this function.
     * @return the neutral element for the comparison
     */
    virtual double getComparisonNeutral() const = 0;

    /**
     * Compare wether a candidate task is better for removal than the
     * currently selected task.
     * @param current the value of the currently selected task
     * @param candidate the value of another candidate
     * @return true, if the candidate is fit better for removal than the
     * current task.
     */
    virtual bool compare(double current, double candidate) const = 0;

    /**
     * Calculate the comparison value of a job
     * @param time
     * @param job the job
     * @return the value of the job
     */
    virtual double calcValue(int time, const Job *job) const = 0;

    /**
     * Check whether the candidate may be cancelled based on its value.
     * This is a default implemenation that may be overwritten in
     * subclasses.
     * @param value
     * @return true, if the candidate can be cancelled
     */
    virtual bool isCancelCandidate(double value) const { return true; }
  };
  

} // NS tmssim

#endif // !SCHEDULERS_OEDF_H
