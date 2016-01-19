/**
 * $Id: icloneable.h 498 2014-05-07 08:05:21Z klugeflo $
 * @file icloneable.h
 * @brief Interface for cloning of objects
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef CORE_ICLONEABLE_H
#define CORE_ICLONEABLE_H 1

namespace tmssim {

  /**
    Interface class for objects that have to provide replication functionality
    Template parameter T should be the name of the class that is going to
    provide this functionality
  */
  template <class T>
    class ICloneable {
  public:
    
    /**
     * Clones an object
     * @return A pointer to a deep-copied clone of the object
     */
    virtual T* clone() const = 0;
    
    /**
     * D'tor
     */
    virtual ~ICloneable() {}
  };

} // NS tmssim

#endif /* CORE_ICLONEABLE_H */

