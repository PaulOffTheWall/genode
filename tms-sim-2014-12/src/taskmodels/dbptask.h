/**
 * $Id: dbptask.h 815 2014-12-15 13:40:52Z klugeflo $
 * @file dbptask.h
 * @brief Task with distance-based priority assignment
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKMODELS_DBPTASK_H
#define TASKMODELS_DBPTASK_H 1

#include <taskmodels/mktask.h>

namespace tmssim {

  /**
   * @brief Task with distance-based priority assignment.
   * 
   * This is an implementation of the (m,k)-firm task defined
   * in Moncef Hamdaoui, Parameswaran Ramanathan, "A Dynamic Priority
   * Assignment Technique for Streams with (m,k)-Firm Deadlines", IEEE
   * Transactions on Computers, Vol. 44, No. 12, December 1995
   */
  class DBPTask : public MKTask {
  public:
    /**
     * @param _id
     * @param _period
     * @param _et
     * @param _ct
     * @param _uc
     * @param _ua
     * @param _priority this parameter is actually not used as the priority
     * is assigned dynamically
     * @param _m
     * @param _k
     */
    DBPTask(unsigned int _id, int _period, int _et, int _ct,
	    UtilityCalculator* _uc, UtilityAggregator* _ua,
	    unsigned int _priority, unsigned int _m, unsigned int _k);
    DBPTask(const DBPTask& rhs);
    DBPTask(const MKTask* rhs);
    virtual ~DBPTask();


    //virtual void write(xmlTextWriterPtr writer) const;
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

  protected:
    virtual Job* spawnHook(int now);
    virtual bool completionHook(Job *job, int now);
    virtual bool cancelHook(Job *job);
    virtual char getShortId(void);

  private:

    
  };


  /**
   * @brief Generic allocator function
   */
  static MKTask* DBPTaskAllocator(MKTask* task) { return new DBPTask(task); }

} // NS tmssim

#endif // !TASKMODELS_DBPTASK_H
