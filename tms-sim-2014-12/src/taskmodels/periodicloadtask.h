/**
 * $Id: periodicloadtask.h
 * @file periodicloadtask.h
 * @brief Periodic Load task class
 * @author Avinash Kundaliya <avinash.kundaliya@tum.de>
 */

#ifndef TASKMODELS_PERIOlDICLOADTASK_H
#define TASKMODELS_PERIODICLOADTASK_H 1

#include <core/scobjects.h>

namespace tmssim {

  class PeriodicLoadTask : public Task {
  public:
    PeriodicLoadTask(unsigned int _id, int _period, int _et, int _ct,
    UtilityCalculator* __uc, UtilityAggregator* __ua, int o=0, int _prio=1, int _matrixSize=3);
    PeriodicLoadTask(const PeriodicLoadTask& rhs);

    virtual std::ostream& print(std::ostream& ost) const;

    virtual double getLastExecValue(void) const;
    virtual double getHistoryValue(void) const;


    /**
     * Serializes this object with the given xmlTextWriter to a xml-document
     * @param writer A pointer to the xmlWriter that should be used for
     * serialization
     * @todo remove when new XML writer class is integrated
     */
    //void write(xmlTextWriterPtr writer) const;

    /**
     * Creates a deep-copy of the current task
     * @return A pointer to a new object with the same properties of this object
     */
    virtual Task* clone() const;

    /**
     * @name XML
     * @{
     */
    virtual const std::string& getClassElement();
    virtual int writeData(xmlTextWriterPtr writer);
    /**
     * @}
     */

    int getPeriod() const { return period; }
    int getOffset() const { return offset; }

  protected:
    virtual int startHook(int now);
    virtual Job* spawnHook(int now);
    virtual int getNextActivationOffset(int now);
    virtual bool completionHook(Job *job, int now);
    virtual bool cancelHook(Job *job);
    virtual double calcExecValue(const Job *job, int complTime) const;
    virtual double calcHistoryValue(const Job *job, int complTime) const;
    virtual double calcFailHistoryValue(const Job *job) const;

    virtual void advanceHistory(void);

    virtual char getShortId(void) const;

    int period;
    int offset; ///< offset for periodic activation
    int matrixSize;
    double lastUtility;
    double historyUtility;
  };
  
} // NS tmssim

#endif /* !TASKMODELS_PERIODICTASK_H */
  
