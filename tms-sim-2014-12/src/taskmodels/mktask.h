/**
 * $Id: mktask.h 740 2014-09-30 15:05:44Z klugeflo $
 * @file mktask.h
 * @brief General task with (m,k)-firm real-time constraints
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKMODELS_MKTASK_H
#define TASKMODELS_MKTASK_H 1

#include <core/scobjects.h>
#include <taskmodels/periodictask.h>
#include <taskmodels/mkmonitor.h>

namespace tmssim {

  /**
   * @brief General task with (m,k)-firm real-time constraints
   *
   * This class acts as superclass for the implementation of task models
   * with (m,k)-firm real-time constraints.
   *
   * @todo
   * - add constructor from tmssim::PeriodicTask
   * - add mktaskbuilder (?), extend xml schema by mktask
   */
  class MKTask : public PeriodicTask {
  public:
    /**
     * @param _id
     * @param _period
     * @param _et
     * @param _ct
     * @param _uc
     * @param _ua
     * @param _priority
     * @param _m
     * @param _k
     * @param _spin The spin parameter used by the Pattern-Spinning algorithm
     * for (m,k)-tasks by Semprebom et al. 2013, earlier by Quan & Hu 2000
     */
    MKTask(unsigned int _id, int _period, int _et, int _ct,
	   UtilityCalculator* _uc, UtilityAggregator* _ua,
	   unsigned int _priority, unsigned int _m, unsigned int _k,
	   unsigned int _spin = 0);
    MKTask(const MKTask& rhs);
    MKTask(const MKTask* rhs);
    virtual ~MKTask();

    virtual std::ostream& print(std::ostream& ost) const;

    /**
     * @brief (m,k)-constrained tasks can have a failure-state distance
     * different from 1.
     *
     * Overrides the Task::getDistance() method. This method implements
     * the distance calculation for (m,k)-constrained real-time tasks
     * as defined by Hamdaoui & Ramanathan (1995) in equation 3.1:
     * \f$\mbox{priority}_{i+1}^j=k-l_j(m_j,s)+1\f$
     *
     * @return The distance to a failure-state.
     */
    int getDistance() const;


    /**
     * @brief Find position of nth deadline meet.
     *
     * Implements function \f$l_j(n,s)\f$ from Hamdaoui & Ramanathan (1995).
     *
     * @param n which deadline meet should be found
     * @param vals ring buffer length k
     * @param pos current head of buffer
     * @return a number between 1 and k, k+1 if no nth meet exists,
     *         and -1 if n>k
     */
    int calcL(int n, const unsigned int* vals, unsigned int pos) const;


    /**
     * @brief simplified calculation of l
     * @param n which deadline meet should be found
     * @return a number between 1 and k, k+1 if no nth meet exists,
     *         and -1 if n>k
     */
    int calcL(int n) const;


    /**
     * @brief get the monitor of this task
     * @return the task's MKMonitor
     */
    const MKMonitor& getMonitor() const;


    /**
     * @name XML
     * @{
     */
    virtual const std::string& getClassElement();
    virtual int writeData(xmlTextWriterPtr writer);
    /**
     * @}
     */

    unsigned int getM() const { return m; }
    unsigned int getK() const { return k; }
    unsigned int getSpin() const { return spin; }
    void setSpin(unsigned int _spin) { spin = _spin; }

    virtual std::string strState() const;

  protected:
    /**
     * If you overwrite this function in your implementation, make sure to
     * still call it!
     */
    virtual bool completionHook(Job *job, int now);

    /**
     * If you overwrite this function in your implementation, make sure to
     * still call it!
     */
    virtual bool cancelHook(Job *job);

    virtual char getShortId(void) const;


    unsigned int m;
    unsigned int k;
    unsigned int spin;

    MKMonitor monitor;

  };

  /**
   * @brief Generic allocator function
   */
  static MKTask* MKTaskAllocator(MKTask* task) { return new MKTask(task); }


} // NS tmssim

#endif // !TASKMODELS_MKTASK_H
