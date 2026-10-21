#include "gestioninterventions.h"
#include "connection.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Une seule instance de Connection pour tout le projet (Singleton)
    Connection &c = Connection::createInstance();
    bool test = c.createConnection();

    if (!test) {
        QMessageBox::critical(nullptr, "Base de données",
                              "Échec de la connexion à la base de données.\n" + c.lastError(),
                              QMessageBox::Cancel);
        return 1;
    }

    GestionInterventions w;
    w.show();
    QMessageBox::information(&w, "Base de données",
                             "Connexion réussie.\nCliquez sur Annuler pour continuer.",
                             QMessageBox::Cancel);
    return a.exec();
}
