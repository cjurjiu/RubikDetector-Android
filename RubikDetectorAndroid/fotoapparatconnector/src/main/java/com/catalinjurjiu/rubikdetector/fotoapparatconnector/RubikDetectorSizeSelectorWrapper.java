package com.catalinjurjiu.rubikdetector.fotoapparatconnector;

import android.support.annotation.NonNull;

import java.util.Collection;

import io.fotoapparat.parameter.Size;
import io.fotoapparat.parameter.selector.SelectorFunction;

/**
 * Wrapper over a {@link SelectorFunction}<{@link Collection}<{@link Size}>, {@link Size}> which notifies a {@link SizeSelectionListener} of the result of
 * its {@link #select} method.
 * <p>
 * This object has no selection logic itself. It relies entirely on the wrapped object to select a certain {@link Size}. This wrapper just
 * notifies its listener of the selected size, then returns the same size from its own {@link #select} method.
 * <p>
 * The listener will be notified synchronously on the same thread on which this object's {@link #select} method is called.
 */
public class RubikDetectorSizeSelectorWrapper implements SelectorFunction<Collection<Size>, Size> {

    private final SelectorFunction<Collection<Size>, Size> selectorFunction;
    private final SizeSelectionListener listener;

    /**
     * Creates a new wrapper.
     *
     * @param selectorFunction {@link SelectorFunction}<{@link Collection}<{@link Size}>, {@link Size}> to be wrapped. This is the object that needs to compute
     *                         the actual size selection logic.
     * @param listener         listener to be notified when the wrapped selector has selected a size.
     */
    public RubikDetectorSizeSelectorWrapper(@NonNull SelectorFunction<Collection<Size>, Size> selectorFunction, @NonNull SizeSelectionListener listener) {
        this.selectorFunction = selectorFunction;
        this.listener = listener;
    }

    @Override
    public Size select(Collection<Size> collection) {
        Size selected = selectorFunction.select(collection);
        listener.onSizeSelected(selected);
        return selected;
    }

    /**
     * Listener that gets notified when a {@link Size} is selected by the {@link RubikDetectorSizeSelectorWrapper}.
     */
    public interface SizeSelectionListener {
        /**
         * Called when a size has been selected.
         *
         * @param size the selected {@link Size}
         */
        void onSizeSelected(Size size);
    }
}
