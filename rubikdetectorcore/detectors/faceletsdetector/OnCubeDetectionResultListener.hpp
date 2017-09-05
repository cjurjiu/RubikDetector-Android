//
// Created by catalin on 12.07.2017.
//

#ifndef RUBIKDETECTORDEMO_ONCUBEDETECTIONRESULTLISTENER_HPP
#define RUBIKDETECTORDEMO_ONCUBEDETECTIONRESULTLISTENER_HPP

class RubikFacelet;

class OnCubeDetectionResultListener {
public:

    OnCubeDetectionResultListener() {};

    virtual ~OnCubeDetectionResultListener() {};

    virtual void
    onCubeDetectionResult(const std::vector<std::vector<RubikFacelet>> &result) const = 0;
};

#endif //RUBIKDETECTORDEMO_ONCUBEDETECTIONRESULTLISTENER_HPP
