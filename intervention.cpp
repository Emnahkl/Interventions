#include "intervention.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

namespace {

QString g_erreur;

bool executer(QSqlQuery &q)
{
    if (!q.exec()) {
        g_erreur = q.lastError().text();
        qDebug() << "Erreur SQL :" << g_erreur;
        return false;
    }
    g_erreur.clear();
    return true;
}

const char *FORMAT_DATE_SQL = "yyyy-MM-dd";
const char *FORMAT_HEURE    = "HH:mm";

} // namespace

Intervention::Intervention()
    : m_id(0), m_date(QDate::currentDate()), m_heure(QTime::currentTime()), m_idZone(0)
{
}

Intervention::Intervention(int id, const QString &type, const QString &adresse,
                           const QDate &date, const QTime &heure,
                           const QString &gravite, const QString &statut, int idZone)
    : m_id(id), m_type(type), m_adresse(adresse), m_date(date), m_heure(heure),
      m_gravite(gravite), m_statut(statut), m_idZone(idZone)
{
}

QString Intervention::derniereErreur()
{
    return g_erreur;
}

// ---------------------------------------------------------------- CREATE
// L'identifiant est généré par Oracle (séquence + trigger), on ne l'envoie pas.
bool Intervention::ajouter()
{
    QSqlQuery q;
    q.prepare("INSERT INTO INTERVENTIONS "
              "(TYPE_INTERVENTION, ADRESSE, DATE_INTERVENTION, HEURE, GRAVITE, STATUT, ID_ZONE) "
              "VALUES (:t, :a, TO_DATE(:d, 'YYYY-MM-DD'), :h, :g, :s, :z)");
    q.bindValue(":t", m_type);
    q.bindValue(":a", m_adresse);
    q.bindValue(":d", m_date.toString(FORMAT_DATE_SQL));
    q.bindValue(":h", m_heure.toString(FORMAT_HEURE));
    q.bindValue(":g", m_gravite);
    q.bindValue(":s", m_statut);
    q.bindValue(":z", m_idZone);
    return executer(q);
}

// ---------------------------------------------------------------- UPDATE
bool Intervention::modifier()
{
    QSqlQuery q;
    q.prepare("UPDATE INTERVENTIONS SET "
              "TYPE_INTERVENTION = :t, ADRESSE = :a, "
              "DATE_INTERVENTION = TO_DATE(:d, 'YYYY-MM-DD'), HEURE = :h, "
              "GRAVITE = :g, STATUT = :s, ID_ZONE = :z "
              "WHERE ID_INTERVENTION = :id");
    q.bindValue(":t", m_type);
    q.bindValue(":a", m_adresse);
    q.bindValue(":d", m_date.toString(FORMAT_DATE_SQL));
    q.bindValue(":h", m_heure.toString(FORMAT_HEURE));
    q.bindValue(":g", m_gravite);
    q.bindValue(":s", m_statut);
    q.bindValue(":z", m_idZone);
    q.bindValue(":id", m_id);
    if (!executer(q))
        return false;
    if (q.numRowsAffected() == 0) {
        g_erreur = "Intervention introuvable.";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------- DELETE
bool Intervention::supprimer(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM INTERVENTIONS WHERE ID_INTERVENTION = :id");
    q.bindValue(":id", id);
    if (!executer(q))
        return false;
    if (q.numRowsAffected() == 0) {
        g_erreur = "Intervention introuvable.";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------- READ
QVector<Intervention> Intervention::afficher(const QString &gravite,
                                             const QString &statut,
                                             const QString &recherche)
{
    QString sql =
        "SELECT ID_INTERVENTION, TYPE_INTERVENTION, ADRESSE, "
        "TO_CHAR(DATE_INTERVENTION, 'YYYY-MM-DD'), HEURE, GRAVITE, STATUT, ID_ZONE "
        "FROM INTERVENTIONS";

    QStringList conditions;
    if (!gravite.isEmpty())
        conditions << "GRAVITE = :g";
    if (!statut.isEmpty())
        conditions << "STATUT = :s";
    const QString r = recherche.trimmed();
    if (!r.isEmpty())
        conditions << "(UPPER(TYPE_INTERVENTION) LIKE UPPER(:r1) "
                      "OR UPPER(ADRESSE) LIKE UPPER(:r2) "
                      "OR TO_CHAR(ID_INTERVENTION) LIKE :r3)";
    if (!conditions.isEmpty())
        sql += " WHERE " + conditions.join(" AND ");
    sql += " ORDER BY ID_INTERVENTION";

    QSqlQuery q;
    q.prepare(sql);
    if (!gravite.isEmpty())
        q.bindValue(":g", gravite);
    if (!statut.isEmpty())
        q.bindValue(":s", statut);
    if (!r.isEmpty()) {
        const QString motif = "%" + r + "%";
        q.bindValue(":r1", motif);
        q.bindValue(":r2", motif);
        q.bindValue(":r3", motif);
    }

    QVector<Intervention> liste;
    if (!executer(q))
        return liste;

    while (q.next()) {
        liste.push_back(Intervention(
            q.value(0).toInt(),
            q.value(1).toString(),
            q.value(2).toString(),
            QDate::fromString(q.value(3).toString(), FORMAT_DATE_SQL),
            QTime::fromString(q.value(4).toString(), FORMAT_HEURE),
            q.value(5).toString(),
            q.value(6).toString(),
            q.value(7).toInt()));
    }
    return liste;
}

// ---------------------------------------------------------------- Zones
QVector<QPair<int, QString> > Intervention::zones()
{
    QVector<QPair<int, QString> > liste;
    QSqlQuery q;
    q.prepare("SELECT ID_ZONE, NOM FROM ZONE_COUVERTURE ORDER BY ID_ZONE");
    if (!executer(q))
        return liste;
    while (q.next())
        liste.push_back(qMakePair(q.value(0).toInt(), q.value(1).toString()));
    return liste;
}
