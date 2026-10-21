#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QString>

// Connexion unique à la base Oracle (patron Singleton, "Meyers Singleton").
class Connection
{
public:
    static Connection& createInstance();   // point d'accès global à l'instance unique
    bool createConnection();               // ouvre la connexion Oracle via ODBC
    void closeConnection();
    QString lastError() const { return m_error; }

private:
    Connection();                           // constructeur privé
    ~Connection();
    Connection(const Connection&) = delete; // pas de copie
    Connection& operator=(const Connection&) = delete;

    QSqlDatabase m_db;
    QString m_error;
};

#endif // CONNECTION_H
