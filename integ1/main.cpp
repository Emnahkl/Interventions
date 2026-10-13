#include "integratedwindow.h"
#include "connexion.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Fire Station");
    QCoreApplication::setApplicationName("GestionVehicules");
    app.setStyle("Fusion");

    if (!Connexion::ouvrir()) {
        return 1;
    }

    IntegratedWindow window;
    window.showMaximized();
    return app.exec();
}

