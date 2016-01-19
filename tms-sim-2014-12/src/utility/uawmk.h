/**
 * $Id: uawmk.h 677 2014-09-09 12:49:34Z klugeflo $
 * @file uawmk.h
 * @brief Utility Aggregator for utility-base (m,k)-firm scheduling
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILITY_UAWMK_H
#define UTILITY_UAWMK_H 1

#include <utility/uawindow.h>

namespace tmssim {

  class UAWMK : public UAWindow {
  public:
    UAWMK(unsigned int _m, unsigned int _k);
    UAWMK(const UAWMK& rhs);
    virtual ~UAWMK();

    unsigned int getM() const { return m; }
    unsigned int getK() const { return k; }

    virtual double getCurrentUtility(void) const;
    virtual double predictUtility(double u) const;

    virtual void write(xmlTextWriterPtr writer) const;
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
    unsigned int m;
    unsigned int k;

    double currentSum;

  };

} // NS tmssim

#endif // !UTILITY_UAWMK_H
