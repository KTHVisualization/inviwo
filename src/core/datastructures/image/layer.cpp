/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Main file authors: Erik Sund�n, Peter Steneteg
 *
 *********************************************************************************/

#include <inviwo/core/datastructures/image/layer.h>
#include <inviwo/core/datastructures/image/layerram.h>

namespace inviwo {

Layer::Layer(uvec2 dimensions, const DataFormatBase* format, LayerType type)
    : Data(format)
    , StructuredGridEntity<2>(dimensions)
    , layerType_(type) {}

Layer::Layer(LayerRepresentation* in)
    : Data(in->getDataFormat())
    , StructuredGridEntity<2>(in->getDimension())
    , layerType_(in->getLayerType()) {
    clearRepresentations();
    addRepresentation(in);
}

Layer::Layer(const Layer& rhs)
    : Data(rhs)
    , StructuredGridEntity<2>(rhs)
    , layerType_(rhs.layerType_) {}

Layer& Layer::operator=(const Layer& that) {
    if (this != &that) {
        Data::operator=(that);
        StructuredGridEntity<2>::operator=(that);
        layerType_ = that.layerType_;
    }

    return *this;
}

Layer* Layer::clone() const {
    return new Layer(*this);
}

Layer::~Layer() {
    // Representations are deleted by Data destructor.
}



void Layer::resize(uvec2 dimensions) {
    setDimension(dimensions);

    if (lastValidRepresentation_) {
        // Resize last valid representation and remove the other ones
        static_cast<LayerRepresentation*>(lastValidRepresentation_)->resize(dimensions);
        std::vector<DataRepresentation*>::iterator it = std::find(representations_.begin(), representations_.end(), lastValidRepresentation_);

        // First delete the representations before erasing them from the vector
        for (size_t i=0; i<representations_.size(); i++) {
            if (representations_[i] != lastValidRepresentation_) {
                delete representations_[i];
                representations_[i] = NULL;
            }
        }

        // Erasing representations: start from the back
        if (it != --representations_.end()) {
            std::vector<DataRepresentation*>::iterator eraseFrom = it + 1;
            representations_.erase(eraseFrom, representations_.end());
        }

        // and then erase the ones infron of the valid representation
        if (representations_.begin() != it)
            representations_.erase(representations_.begin(), it);
    }

    setAllRepresentationsAsInvalid();
}


uvec2 Layer::getDimension() const {
    return StructuredGridEntity<2>::getDimension();
}
void  Layer::setDimension(const uvec2& dim) {
    StructuredGridEntity<2>::setDimension(dim);
}

void Layer::resizeRepresentations(Layer* targetLayer, uvec2 targetDim) {
    //TODO: check if getClassName() is necessary.
    //TODO: And also need to be tested on multiple representations_ such as LayerRAM, LayerDisk etc.,
    //TODO: optimize the code
    targetLayer->resize(targetDim);
    LayerRepresentation* layerRepresentation = 0;
    LayerRepresentation* targetRepresentation = 0;
    std::vector<DataRepresentation*>& targetRepresentations = targetLayer->representations_;

    if (targetRepresentations.size()) {
        for (int i=0; i<static_cast<int>(representations_.size()); i++) {
            layerRepresentation = static_cast<LayerRepresentation*>(representations_[i]);

            if (isRepresentationValid(i)) {
                int numberOfTargets = static_cast<int>(targetRepresentations.size());

                for (int j=0; j<numberOfTargets; j++) {
                    targetRepresentation = static_cast<LayerRepresentation*>(targetRepresentations[j]);

                    if (typeid(*layerRepresentation)==typeid(*targetRepresentation)) {
                        if (layerRepresentation->copyAndResizeLayer(targetRepresentation)) {
                            targetLayer->setRepresentationAsValid(j);
                            targetLayer->lastValidRepresentation_ = targetRepresentations[j];
                        }
                    }
                }
            }
        }
    }
    else {
        ivwAssert(lastValidRepresentation_!=0, "Last valid representation is expected.");
        targetLayer->setAllRepresentationsAsInvalid();
        targetLayer->createDefaultRepresentation();
        LayerRepresentation* lastValidRepresentation = dynamic_cast<LayerRepresentation*>(lastValidRepresentation_);
        LayerRepresentation* cloneOfLastValidRepresentation = dynamic_cast<LayerRepresentation*>(lastValidRepresentation->clone());
        targetLayer->addRepresentation(cloneOfLastValidRepresentation);
        targetLayer->resize(targetDim);

        if (lastValidRepresentation->copyAndResizeLayer(cloneOfLastValidRepresentation)) {
            targetLayer->setRepresentationAsValid(static_cast<int>(targetLayer->representations_.size())-1);
            targetLayer->lastValidRepresentation_ = cloneOfLastValidRepresentation;
        }
    }
}

LayerType Layer::getLayerType() const {
    return layerType_;
}

DataRepresentation* Layer::createDefaultRepresentation() {
    return createLayerRAM((uvec2)getDimension(), getLayerType(), getDataFormat());
}


} // namespace
