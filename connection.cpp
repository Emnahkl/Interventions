#include "connection.h"

#include <QDebug>
#include <QSqlError>

// ======= A MODIFIER SI BESOIN (mêmes valeurs que votre source ODBC) =======
static const char *SOURCE_ODBC  = "Source_Projet2A";
static const char *UTILISATEUR  = "emna";
static const char *MOT_DE_PASSE = "esprit18";   // ne jamais pousser le vrai mot de passe sur GitHub
// ==========================================================================

Connection::Connection()
{
    m_db = QSqlDatabase::addDatabase("QODBC");
}

Connection::~Connection()
{
    closeConnection();
}

Connection& Connection::createInstance()
{
    static Connection instance;   // créée au 1er appel, vit jusqu'à la fin du programme
    return instance;
}

bool Connection::createConnection()
{
    m_db.setDatabaseName(SOURCE_ODBC);
    m_db.setUserName(UTILISATEUR);
    m_db.setPassword(MOT_DE_PASSE);

    if (m_db.open()) {
        qDebug() << "Connexion établie";
        return true;
    }
    m_error = m_db.lastError().text();
    qDebug() << "Échec de la connexion :" << m_error;
    return false;
}

void Connection::closeConnection()
{
    if (m_db.isOpen())
        m_db.close();
}
