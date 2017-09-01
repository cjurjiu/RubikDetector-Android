package com.catalinjurjiu.rubikdetector.fotoapparatconnector;

import android.support.annotation.NonNull;

import java.util.Collection;

import io.fotoapparat.parameter.Size;
import io.fotoapparat.parameter.selector.SelectorFunction;

/**
 * Created by catalin on 01.09.2017.
 */

public class RubikDetectorSizeSelector implements SelectorFunction<Size> {

    private final SelectorFunction<Size> customSelectorFunction;
    private final SizeSelectionListener listener;

    public RubikDetectorSizeSelector(@NonNull SelectorFunction<Size> customSelectorFunction, @NonNull SizeSelectionListener listener) {
        this.customSelectorFunction = customSelectorFunction;
        this.listener = listener;
    }

    @Override
    public Size select(Collection<Size> collection) {
        Size selected = customSelectorFunction.select(collection);
        listener.onSizeSelected(selected);
        return selected;
    }

    public interface SizeSelectionListener {
        void onSizeSelected(Size size);
    }
}
