//
// Created by catalin on 05.09.2017.
//

#ifndef RUBIKDETECTOR_IMAGEPROCESSOR_HPP
#define RUBIKDETECTOR_IMAGEPROCESSOR_HPP

#include "../misc/Debuggable.hpp"

namespace rbdt {

/**
 * Interface which defines the basic capabilities of an Image Processor capable of detecting Rubik's Cube facelets.
 *
 * Implementations of this processor are required to be able to support, or partially support,
 * the data formats defined in ImageProcessor::ImageFormat, as both input & output. Additionally, it needs to support changes both to the
 * data format & to the size of the processed frame, after it has been instantiated, through ImageProcessor::updateImageProperties().
 *
 * Frames will be processed through the ImageProcessor::process() method. This processor is designed with performance in mind. Hence,
 * ImageProcessor::process() is expected to receive a pointer to an unsigned int array large enough to store:
 *   - the input frame;
 *   - the output frame;
 *   - certain additional frames used internally during processing.
 *
 * Because of these stringent memory requirements, each Processor implementation needs to compute its specific memory requirements and make them available
 * to clients through ImageProcessor::getRequiredMemory(). Since these requirements are dependent on the size & format of the input & output frames,
 * he value returned by ImageProcessor::getRequiredMemory() is expected to change  after a call to ImageProcessor::updateImageProperties().
 *
 * How the data will be organized by the Processor, within the array passed to ImageProcessor::process(), is implementation specific. However,
 * each subclass needs to be able to provide two things regarding this data:
 *  - the expected location of the <b>input</b> frame array, in the array passed to ImageProcessor::process();
 *    * this is achieved through ImageProcessor::getInputFrameBufferOffset().
 *  - the expected location of the <b>output</b> frame array, in the array passed to ImageProcessor::process().
 *    * this is achieved through ImageProcessor::getOutputFrameBufferOffset().
 *
 * Additionally, it the processor also exposes the capability of the input frame to be overriden by the output frame, through
 * ImageProcessor::overrideInputFrameWithOutputFrame().
 *
 */
template<typename INPUT_TYPE, typename IMAGE_PROPERTIES_TYPE, typename RESULT_TYPE>
class ImageProcessor : public Debuggable {
public:

    /**
     * Empty virtual destructor. Technically not necessary to be explicitly declared.
     */
    virtual ~ImageProcessor() {}

    /**
     * Processes the data present in the byte array. It expects the input frame data to be at the offset returned by ImageProcessor::getInputFrameBufferOffset(), and
     * to have a byte length equal to ImageProcessor::getInputFrameByteCount().
     *
     * If the input frame is modified during processing, the results of these modifications will be present as an "output frame", which will be written in the input
     * frame data array at an offset equal to ImageProcessor::getOutputFrameBufferOffset(), and will have a byte length equal to ImageProcessor::getOutputFrameByteCount().
     *
     * @param [in/out] imageData.
     * @return nullptr if facelets are not detected in the current frame, or a 3x3 std::vector<> of RubikFacelet which contains the found facelets.
     */
    virtual RESULT_TYPE process(INPUT_TYPE imageData)=0;

    /**
     * Use to notify that the frame size, or the format of the either the input or output frame has changed.
     *
     * This causes the processor to recompute its required memory & update the memory layout used when ImageProcessor::process() is called. As a
     * consequence, following this call, the output of the following methods might change:
     *   - ImageProcessor::getRequiredMemory()
     *   - ImageProcessor::getOutputFrameBufferOffset()
     *   - ImageProcessor::getOutputFrameByteCount()
     *   - ImageProcessor::getInputFrameByteCount()
     *   - ImageProcessor::getInputFrameBufferOffset()
     *
     * @param imageProperties an instance of updated ImageProperties.
     */
    virtual void updateImageProperties(IMAGE_PROPERTIES_TYPE imageProperties)=0;

    /**
     * Overrides the input frame data from the byte array received as a parameters, with the output frame data.
     *
     * If the input & output frames are stored in different image formats, then the output frame
     * is first converted to the format of the input frame, and only after its data is copied over the input frame data.
     *
     * This allocates no new memory, i.e. the converted output frame will be written in the byte array received as a parameter at
     * offset ImageProcessor::getInputFrameBufferOffset() and will have ImageProcessor::getInputFrameByteCount() bytes.
     *
     * @param imageData byte array which stores at least the input & output frame data, has a size equal to ImageProcessor::getRequiredMemory()
     * and has the input & output frames located at the locations provided by the methods in the references
     *
     * @see ImageProcessor::getOutputFrameBufferOffset()
     * @see ImageProcessor::getOutputFrameByteCount()
     * @see ImageProcessor::getInputFrameByteCount()
     * @see ImageProcessor::getInputFrameBufferOffset()
     */
    virtual void overrideInputFrameWithOutputFrame(INPUT_TYPE imageData)=0;

    /**
     * Returns the required size in bytes of the array passed to ImageProcessor::process(), given the ImageProperties currently set on the Processor.
     *
     * The value returned here is recomputed each time ImageProcessor::updateImageProperties() is called.
     *
     * @return required bytes, as an int.
     */
    virtual int getRequiredMemory()=0;

    /**
     * Returns the input data offset in bytes, relative to the start of the byte array provided in ImageProcessor::process().
     *
     * The value returned here is recomputed each time ImageProcessor::updateImageProperties() is called.
     *
     * @return int frame offset, in bytes.
     */
    virtual int getInputFrameBufferOffset()=0;

    /**
     * Returns the expected size in bytes of the input frame.
     *
     * The value returned here is recomputed each time ImageProcessor::updateImageProperties() is called.
     *
     * @return int expected input frame size, in bytes.
     */
    virtual int getInputFrameByteCount()=0;

    /**
     * Returns the output data offset in bytes, relative to the start of the byte array provided in ImageProcessor::process().
     *
     * The value returned here is recomputed each time ImageProcessor::updateImageProperties() is called.
     *
     * @return int frame offset, in bytes.
     * */
    virtual int getOutputFrameBufferOffset()=0;

    /**
     * Returns the expected size in bytes of the output frame.
     *
     * The value returned here is recomputed each time ImageProcessor::updateImageProperties() is called.
     *
     * @return int expected output frame size, in bytes.
     */
    virtual int getOutputFrameByteCount()=0;

};
} //end namespace rbdt
#endif //RUBIKDETECTOR_IMAGEPROCESSOR_HPP
