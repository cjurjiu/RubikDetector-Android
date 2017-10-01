//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTOR_DEBUGGABLE_HPP
#define RUBIKDETECTOR_DEBUGGABLE_HPP
namespace rbdt {

/**
 * Denotes an object capable of changing its behavior, or logging style, relative to whether it's debuggable or not.
 */
class Debuggable {
public:
    /**
     * Virtual empty destructor.
     */
    virtual ~Debuggable() {}

    /**
     * Call this to change the debug mode of the object.
     * @param debuggable true to set this object as debuggable, false to set it as not debuggable.
     */
    virtual void setDebuggable(const bool debuggable) =0;

    /**
     * Returns true of false depending on whether the instance is debuggable or not.
     * @return true if debuggable, false otherwise.
     */
    virtual bool isDebuggable() const =0;
};
} //end namespace rbdt
#endif //RUBIKDETECTOR_DEBUGGABLE_HPP
