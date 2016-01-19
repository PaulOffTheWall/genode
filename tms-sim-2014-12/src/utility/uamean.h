/**
 * $Id: uamean.h 677 2014-09-09 12:49:34Z klugeflo $
 * @file uamean.h
 * @brief Aggregator with equal weight
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#ifndef UTILITY_UAMEAN_H
#define UTILITY_UAMEAN_H 1

#include <core/scobjects.h>

namespace tmssim {

  class UAMean : public virtual UtilityAggregator {
  public:
    UAMean();
    virtual ~UAMean(void);
    
    virtual double getCurrentUtility(void) const;
    virtual double predictUtility(double u) const;
    
    /**
     * Serializes this object with the given xmlTextWriter to a xml-document
     * @param writer A pointer to the xmlWriter that should be used for serialization
     */
    virtual void write(xmlTextWriterPtr writer) const;
    
    /**
     * Creates a deep-copy of the current object
     * @return A pointer to a new object with the same properties of this object
     */
    virtual UtilityAggregator* clone() const;

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
    virtual void addHook(double u);
  };

} // NS tmssim

#endif /* !UTILITY_UAMEAN_H */
