#ifndef IVW_CANVASPROCESSOR_H
#define IVW_CANVASPROCESSOR_H

#include <inviwo/core/inviwocoredefine.h>
#include <inviwo/core/inviwo.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/util/canvas.h>
#include "inviwo/core/properties/snapshotproperty.h"

namespace inviwo {

class IVW_CORE_API CanvasProcessor : public Processor {
public:
    CanvasProcessor();
    ~CanvasProcessor();
    virtual Processor* create() const;

    virtual void initialize();
    virtual void deinitialize();

    virtual void process();

    void setCanvas(Canvas* canvas) { canvas_ = canvas; }
    Canvas* getCanvas() const { return canvas_; }

    bool isEndProcessor() { return true; }

protected:
    ImagePort inport_;
    SnapshotProperty snapshot_;

private:
    Canvas* canvas_;
};

} // namespace

#endif // IVW_CANVASPROCESSOR_H
