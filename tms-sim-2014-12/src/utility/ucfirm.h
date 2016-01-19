/**
 * $Id: ucfirm.h 677 2014-09-09 12:49:34Z klugeflo $
 * @file ucfirm.h
 * @brief Firm real-time utility calculator
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#ifndef UTILITY_UCFIRM_H
#define UTILITY_UCFIRM_H 1

#include <core/scobjects.h>

namespace tmssim {

  /**
    TUF for firm real-time
  */
  class UCFirmRT : public UtilityCalculator {
  public:
    UCFirmRT();
    virtual ~UCFirmRT(void);
    virtual double calcUtility(const Job *job, int complTime) const;
    
    /**
     * Serializes this object with the given xmlTextWriter to a xml-document
     * @param writer A pointer to the xmlWriter that should be used for serialization
     */
    virtual void write(xmlTextWriterPtr writer) const;
    
    /**
     * Creates a deep-copy of the current object
     * @return A pointer to a new object with the same properties of this object
     */
    virtual UtilityCalculator* clone() const;

    /**
     * @name XML
     * @{
     */

    virtual const std::string& getClassElement();
    virtual int writeData(xmlTextWriterPtr writer);

    /**
     * @}
     */
    
  };

} // NS tmssim

#endif /* !UTILITY_UCFIRM_H */
