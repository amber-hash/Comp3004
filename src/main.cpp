#include <QApplication>
#include "ui/MainWindow.h"
#include "data/DataStore.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("HintonMarket");
    app.setApplicationDisplayName("HintonMarket — Hintonville Farmers Market");
    app.setOrganizationName("Hintonville");

    // Initialize in-memory data store
    DataStore::instance().initialize();

    MainWindow window;
    window.show();

    return app.exec();
}