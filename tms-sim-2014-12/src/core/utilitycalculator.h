/**
 * $Id: utilitycalculator.h 815 2014-12-15 13:40:52Z klugeflo $
 * @file utilitycalculator.h
 * @brief Calculation of single-job utilities
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_UTILITYCALCULATOR_H
#define CORE_UTILITYCALCULATOR_H 1

#include <core/writeabletoxml.h>
//#include <core/iwriteabletoxml.h>
#include <core/icloneable.h>

namespace tmssim {

  class Job;

  /**
   * @brief Calculation of single-job utilities
   */
  class UtilityCalculator
    : //public IWriteableToXML,
    public WriteableToXML,
    public ICloneable<UtilityCalculator>
    {
  public:
    /*
    UtilityCalculator();
    UtilityCalculator(const UtilityCalculator& rhs);
    */
    virtual ~UtilityCalculator(void) = 0;
    
    /**
      Calculate the single-execution utility
      @param job the job whose utility shall be calculated
      @param complTime actual or estimated completion time
      @return a utility value
    */
    virtual double calcUtility(const Job *job, int complTime) const = 0;
    virtual UtilityCalculator* clone() const = 0;

    /**
     * @name XML
     * @{
     */

    virtual int writeData(xmlTextWriterPtr writer);

    /**
     * @}
     */

  };

} // NS tmssim

#endif // !CORE_UTILITYCALCULATOR_H
