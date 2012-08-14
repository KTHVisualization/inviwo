#ifndef IVW_INVIWOAPPLICATIONQT_H
#define IVW_INVIWOAPPLICATIONQT_H

#include <QApplication>
#include <QMainWindow>

#include "inviwo/core/inviwoapplication.h"

namespace inviwo {

    class InviwoApplicationQt : public InviwoApplication, public QApplication {

public:
    InviwoApplicationQt(std::string displayName_, std::string basePath_,
                        int argc, char** argv);
    virtual ~InviwoApplicationQt();

    void setMainWindow(QMainWindow* mainWindow) { mainWindow_ = mainWindow; }
    QMainWindow* getMainWindow() { return mainWindow_; }

private:
    QMainWindow* mainWindow_;
};

} // namespace

#endif // IVW_INVIWOAPPLICATIONQT_H
