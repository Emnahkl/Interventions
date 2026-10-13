#include "integratedwindow.h"
#include "connexion.h"

#include <QApplication>
#include <QMessageBox>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Fire Station");
    QCoreApplication::setApplicationName("GestionVehicules");
    app.setStyle("Fusion");

    if (!Connexion::ouvrir()) {
        QMessageBox::critical(nullptr, "Base de données",
                              "Impossible d'ouvrir la base de données des équipements.");
        return 1;
    }

    IntegratedWindow window;
    window.showMaximized();
    return app.exec();
}

