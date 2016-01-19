/**
 * $Id: mkctask.h 815 2014-12-15 13:40:52Z klugeflo $
 * @file mkctask.h
 * @brief (m,k)-firm real-time task for control applications
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKMODELS_MKCTASK_H
#define TASKMODELS_MKCTASK_H 1

#include <core/scobjects.h>
#include <taskmodels/mktask.h>
#include <taskmodels/mkmonitor.h>

namespace tmssim {

  /**
   * Implementation of a (m,k)-firm real-time task as defined
   * by Parameswaran Ramanathan in "Overload Management in Real-Time
   * Control Applications Using (m,k)-Firm Guarantee" (IEEE Transactions
   * on Parallel and Distributed Systems, vol. 10, No. 6, June 1999)
   */
  class MKCTask : public MKTask {
  public:
    MKCTask(unsigned int _id, int _period, int _et, int _ct,
	    UtilityCalculator* _uc, UtilityAggregator* _ua,
	    unsigned int _priority, unsigned int _m, unsigned int _k,
	    unsigned int _spin = 0, bool _useSpin = false);
    MKCTask(const MKCTask& rhs);
    MKCTask(const MKTask* rhs);
    virtual ~MKCTask();

    //virtual void write(xmlTextWriterPtr writer) const;
    virtual Task* clone() const;

    bool isJobMandatory(unsigned int jobId) const;

    void enableSpin();
    void disableSpin();

    /**
     * @name XML
     * @{
     */
    virtual const std::string& getClassElement();
    virtual int writeData(xmlTextWriterPtr writer);
    /**
     * @}
     */

  protected:
    virtual Job* spawnHook(int now);
    virtual bool completionHook(Job *job, int now);
    virtual bool cancelHook(Job *job);
    virtual char getShortId(void);

  private:
    // this is the spin parameter actually used - it is only != 0 if _useSpin was set in the contstructor
    unsigned int actSpin;
    //unsigned int m;
    //unsigned int k;
    //unsigned int a;

    //MKMonitor *monitor;

  };


  /**
   * @brief Generic allocator function
   * @return MKCTask that does not use the spin parameter
   */
  static MKTask* MKCTaskAllocator(MKTask* task) { return new MKCTask(task); }


  /**
   * @brief Generic allocator function
   * @return MKCTask that uses the spin parameter
   */
  static MKTask* MKCTaskWithSpinAllocator(MKTask* task) {
    MKCTask* t = new MKCTask(task);
    t->enableSpin();
    return t;
  }

} // NS tmssim

#endif /* !TASKMODELS_MKCTASK_H */
