//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTOR_DEBUGGABLE_HPP
#define RUBIKDETECTOR_DEBUGGABLE_HPP

class Debuggable {
public:
    virtual ~Debuggable() {

    }

    virtual void setDebuggable(const bool debuggable) =0;

    virtual bool isDebuggable() const =0;
};

#endif //RUBIKDETECTOR_DEBUGGABLE_HPP
