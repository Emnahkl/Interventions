#ifndef GESTIONINTERVENTIONS_H
#define GESTIONINTERVENTIONS_H

#include <QVector>
#include <QWidget>

#include "intervention.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GestionInterventions; }
QT_END_NAMESPACE

class GestionInterventions : public QWidget
{
    Q_OBJECT

public:
    explicit GestionInterventions(QWidget *parent = nullptr);
    ~GestionInterventions();

private slots:
    void rafraichir();          // recharge le tableau depuis Oracle (filtres + recherche + tri)
    void ajouter();
    void modifier();
    void supprimer();
    void consulter();
    void exporterPdf();
    void trierParDate();
    void trierParGravite();
    void afficherStats();
    void fonctionnaliteAVenir();

private:
    enum Tri { TriId, TriDate, TriGravite };

    bool dialogue(Intervention &it, const QString &titre);
    int idCourant() const;
    void mettreAJourCartes();

    Ui::GestionInterventions *ui;
    Tri m_tri;
    QVector<Intervention> m_liste;
};

#endif // GESTIONINTERVENTIONS_H
