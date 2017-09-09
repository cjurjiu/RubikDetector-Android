//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTOR_ONCUBEDETECTIONRESULTLISTENER_HPP
#define RUBIKDETECTOR_ONCUBEDETECTIONRESULTLISTENER_HPP

class RubikFacelet;

class OnCubeDetectionResultListener {
public:

    OnCubeDetectionResultListener() {};

    virtual ~OnCubeDetectionResultListener() {};

    virtual void
    onCubeDetectionResult(const std::vector<std::vector<RubikFacelet>> &result) const = 0;
};

#endif //RUBIKDETECTOR_ONCUBEDETECTIONRESULTLISTENER_HPP
