/**
 * $Id: uawmean.h 677 2014-09-09 12:49:34Z klugeflo $
 * @file uawmean.h
 * @brief Aggregator over sliding window with average calculation
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#ifndef UTILITY_UAWMEAN_H
#define UTILITY_UAWMEAN_H 1

#include <utility/uawindow.h>

namespace tmssim {

  class UAWMean : public UAWindow {
  public:
    UAWMean(size_t _size);
    virtual ~UAWMean(void);
    
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
    
  private:
    double currentSum;
  };
  
} // NS tmssim

#endif /* !UTILITY_UAWMEAN_H */
