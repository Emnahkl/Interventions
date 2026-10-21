#include "gestioninterventions.h"
#include "ui_gestioninterventions.h"

#include <QColor>
#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFont>
#include <QFormLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPdfWriter>
#include <QTableWidgetItem>
#include <QTextDocument>
#include <QTimeEdit>
#include <algorithm>

namespace {

QStringList listeTypes()
{
    return QStringList() << "Incendie" << "Accident de la route" << "Secours à personne"
                         << "Fuite de gaz" << "Inondation";
}
QStringList listeGravites()
{
    return QStringList() << "Faible" << "Moyenne" << "Élevée" << "Critique";
}
QStringList listeStatuts()
{
    return QStringList() << "En cours" << "En attente" << "Terminée";
}

int rangGravite(const QString &g) { return listeGravites().indexOf(g); }

QColor couleurGravite(const QString &g)
{
    if (g == "Critique") return QColor("#b91c1c");
    if (g == "Élevée")   return QColor("#ea580c");
    if (g == "Moyenne")  return QColor("#b45309");
    return QColor("#15803d");
}

QColor couleurStatut(const QString &s)
{
    if (s == "En cours")   return QColor("#b45309");
    if (s == "En attente") return QColor("#b91c1c");
    return QColor("#15803d");
}

} // namespace

GestionInterventions::GestionInterventions(QWidget *parent)
    : QWidget(parent), ui(new Ui::GestionInterventions), m_tri(TriId)
{
    ui->setupUi(this);

    // Tableau
    ui->tableInterventions->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableInterventions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Boutons -> slots
    connect(ui->btnAjouter,       SIGNAL(clicked()), this, SLOT(ajouter()));
    connect(ui->btnModifier,      SIGNAL(clicked()), this, SLOT(modifier()));
    connect(ui->btnSupprimer,     SIGNAL(clicked()), this, SLOT(supprimer()));
    connect(ui->btnConsulter,     SIGNAL(clicked()), this, SLOT(consulter()));
    connect(ui->btnExportPdf,     SIGNAL(clicked()), this, SLOT(exporterPdf()));
    connect(ui->btnTriDate,       SIGNAL(clicked()), this, SLOT(trierParDate()));
    connect(ui->btnTriGravite,    SIGNAL(clicked()), this, SLOT(trierParGravite()));
    connect(ui->btnStats,         SIGNAL(clicked()), this, SLOT(afficherStats()));
    connect(ui->btnAffectation,   SIGNAL(clicked()), this, SLOT(fonctionnaliteAVenir()));
    connect(ui->btnParcours,      SIGNAL(clicked()), this, SLOT(fonctionnaliteAVenir()));
    connect(ui->btnPower,         SIGNAL(clicked()), this, SLOT(close()));

    // Filtres + recherche
    connect(ui->comboGravite, SIGNAL(currentIndexChanged(int)), this, SLOT(rafraichir()));
    connect(ui->comboStatut,  SIGNAL(currentIndexChanged(int)), this, SLOT(rafraichir()));
    connect(ui->searchEdit,   SIGNAL(textChanged(QString)),     this, SLOT(rafraichir()));

    rafraichir();
}

GestionInterventions::~GestionInterventions()
{
    delete ui;
}

int GestionInterventions::idCourant() const
{
    const int row = ui->tableInterventions->currentRow();
    if (row < 0 || !ui->tableInterventions->item(row, 0))
        return -1;
    return ui->tableInterventions->item(row, 0)->data(Qt::UserRole).toInt();
}

// ------------------------------------------------------------ READ (afficher)
void GestionInterventions::rafraichir()
{
    const QString g = ui->comboGravite->currentIndex() > 0 ? ui->comboGravite->currentText() : QString();
    const QString s = ui->comboStatut->currentIndex() > 0 ? ui->comboStatut->currentText() : QString();

    m_liste = Intervention::afficher(g, s, ui->searchEdit->text());

    if (m_tri == TriDate) {
        std::stable_sort(m_liste.begin(), m_liste.end(),
                         [](const Intervention &a, const Intervention &b) {
                             return a.dateHeure() > b.dateHeure();
                         });
    } else if (m_tri == TriGravite) {
        std::stable_sort(m_liste.begin(), m_liste.end(),
                         [](const Intervention &a, const Intervention &b) {
                             return rangGravite(a.getGravite()) > rangGravite(b.getGravite());
                         });
    }

    QTableWidget *t = ui->tableInterventions;
    t->clearContents();
    t->setRowCount(m_liste.size());
    for (int r = 0; r < m_liste.size(); ++r) {
        const Intervention &it = m_liste[r];
        QStringList cellules;
        cellules << QString::number(it.getId())
                 << it.getType()
                 << it.getAdresse()
                 << it.getDate().toString("dd/MM/yyyy") + " " + it.getHeure().toString("HH:mm")
                 << QString::number(it.getIdZone())
                 << it.getGravite()
                 << it.getStatut();
        for (int c = 0; c < cellules.size(); ++c) {
            QTableWidgetItem *item = new QTableWidgetItem(cellules[c]);
            item->setData(Qt::UserRole, it.getId());
            item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            if (c == 5 || c == 6) {
                item->setForeground(QBrush(c == 5 ? couleurGravite(it.getGravite())
                                                  : couleurStatut(it.getStatut())));
                QFont f = item->font();
                f.setBold(true);
                item->setFont(f);
            }
            t->setItem(r, c, item);
        }
    }
    mettreAJourCartes();
}

// Les cartes comptent TOUTES les interventions (sans filtre)
void GestionInterventions::mettreAJourCartes()
{
    int enCours = 0, enAttente = 0, terminee = 0;
    const QVector<Intervention> toutes = Intervention::afficher();
    for (int i = 0; i < toutes.size(); ++i) {
        const QString &s = toutes[i].getStatut();
        if (s == "En cours") ++enCours;
        else if (s == "En attente") ++enAttente;
        else if (s == "Terminée") ++terminee;
    }
    ui->lblEnCours->setText(QString::number(enCours));
    ui->lblEnAttente->setText(QString::number(enAttente));
    ui->lblTerminee->setText(QString::number(terminee));
}

// ------------------------------------------------------ Formulaire ajout/modif
bool GestionInterventions::dialogue(Intervention &it, const QString &titre)
{
    const QVector<QPair<int, QString> > zones = Intervention::zones();
    if (zones.isEmpty()) {
        QMessageBox::warning(this, titre,
            tr("Aucune zone de couverture dans la base.\n"
               "Exécutez d'abord le script sql/interventions.sql dans SQL Developer.\n%1")
                .arg(Intervention::derniereErreur()));
        return false;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(titre);
    QFormLayout *form = new QFormLayout(&dlg);

    QComboBox *type = new QComboBox;
    type->setEditable(true);
    type->addItems(listeTypes());
    type->setCurrentText(it.getType());

    QLineEdit *adresse = new QLineEdit(it.getAdresse());

    QDateEdit *date = new QDateEdit(it.getDate());
    date->setCalendarPopup(true);
    date->setDisplayFormat("dd/MM/yyyy");

    QTimeEdit *heure = new QTimeEdit(it.getHeure());
    heure->setDisplayFormat("HH:mm");

    QComboBox *zone = new QComboBox;
    for (int i = 0; i < zones.size(); ++i)
        zone->addItem(QString("%1 - %2").arg(zones[i].first).arg(zones[i].second), zones[i].first);
    const int idxZone = zone->findData(it.getIdZone());
    if (idxZone >= 0)
        zone->setCurrentIndex(idxZone);

    QComboBox *gravite = new QComboBox;
    gravite->addItems(listeGravites());
    gravite->setCurrentText(it.getGravite());

    QComboBox *statut = new QComboBox;
    statut->addItems(listeStatuts());
    statut->setCurrentText(it.getStatut());

    form->addRow(tr("Type"), type);
    form->addRow(tr("Adresse"), adresse);
    form->addRow(tr("Date"), date);
    form->addRow(tr("Heure"), heure);
    form->addRow(tr("Zone (ID_zone)"), zone);
    form->addRow(tr("Gravité"), gravite);
    form->addRow(tr("Statut"), statut);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(bb);
    connect(bb, SIGNAL(accepted()), &dlg, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), &dlg, SLOT(reject()));

    // Contrôle de saisie : on réaffiche le formulaire tant que c'est invalide
    while (true) {
        if (dlg.exec() != QDialog::Accepted)
            return false;
        if (type->currentText().trimmed().isEmpty()) {
            QMessageBox::warning(this, titre, tr("Le type est obligatoire."));
            continue;
        }
        if (adresse->text().trimmed().size() < 5) {
            QMessageBox::warning(this, titre, tr("L'adresse est obligatoire (au moins 5 caractères)."));
            continue;
        }
        if (zone->currentIndex() < 0) {
            QMessageBox::warning(this, titre, tr("Choisissez une zone."));
            continue;
        }
        break;
    }

    it.setType(type->currentText().trimmed());
    it.setAdresse(adresse->text().trimmed());
    it.setDate(date->date());
    it.setHeure(heure->time());
    it.setIdZone(zone->currentData().toInt());
    it.setGravite(gravite->currentText());
    it.setStatut(statut->currentText());
    return true;
}

// ------------------------------------------------------------ CREATE
void GestionInterventions::ajouter()
{
    Intervention it;
    it.setType(listeTypes().first());
    it.setGravite(listeGravites().first());
    it.setStatut(listeStatuts().at(1));      // « En attente »
    if (!dialogue(it, tr("Ajouter une intervention")))
        return;
    if (it.ajouter())
        rafraichir();
    else
        QMessageBox::critical(this, tr("Ajouter"),
                              tr("Ajout impossible :\n%1").arg(Intervention::derniereErreur()));
}

// ------------------------------------------------------------ UPDATE
void GestionInterventions::modifier()
{
    const int id = idCourant();
    if (id < 0) {
        QMessageBox::information(this, tr("Modifier"), tr("Sélectionnez d'abord une intervention."));
        return;
    }
    Intervention it;
    bool trouve = false;
    for (int i = 0; i < m_liste.size(); ++i)
        if (m_liste[i].getId() == id) { it = m_liste[i]; trouve = true; break; }
    if (!trouve)
        return;

    if (!dialogue(it, tr("Modifier l'intervention")))
        return;
    if (it.modifier())
        rafraichir();
    else
        QMessageBox::critical(this, tr("Modifier"),
                              tr("Modification impossible :\n%1").arg(Intervention::derniereErreur()));
}

// ------------------------------------------------------------ DELETE
void GestionInterventions::supprimer()
{
    const int id = idCourant();
    if (id < 0) {
        QMessageBox::information(this, tr("Supprimer"), tr("Sélectionnez d'abord une intervention."));
        return;
    }
    if (QMessageBox::question(this, tr("Supprimer"),
                              tr("Supprimer l'intervention %1 ?").arg(id)) != QMessageBox::Yes)
        return;

    if (Intervention::supprimer(id)) {
        rafraichir();
    } else {
        const QString err = Intervention::derniereErreur();
        if (err.contains("02292"))
            QMessageBox::warning(this, tr("Supprimer"),
                tr("Cette intervention est liée à des employés, véhicules ou équipements "
                   "(tables PARTICIPER_UTILISER / NECESSITER).\n"
                   "Supprimez d'abord ces liens, ou ajoutez ON DELETE CASCADE dans ces tables."));
        else
            QMessageBox::critical(this, tr("Supprimer"),
                                  tr("Suppression impossible :\n%1").arg(err));
    }
}

// ------------------------------------------------------------ Consulter
void GestionInterventions::consulter()
{
    const int id = idCourant();
    if (id < 0) {
        QMessageBox::information(this, tr("Consulter"), tr("Sélectionnez d'abord une intervention."));
        return;
    }
    for (int i = 0; i < m_liste.size(); ++i) {
        if (m_liste[i].getId() != id)
            continue;
        const Intervention &it = m_liste[i];
        QString nomZone;
        const QVector<QPair<int, QString> > zones = Intervention::zones();
        for (int z = 0; z < zones.size(); ++z)
            if (zones[z].first == it.getIdZone()) nomZone = zones[z].second;
        QMessageBox::information(this, tr("Intervention %1").arg(id),
            tr("Type : %1\nAdresse : %2\nDate : %3 %4\nZone : %5 (%6)\nGravité : %7\nStatut : %8")
                .arg(it.getType(), it.getAdresse(),
                     it.getDate().toString("dd/MM/yyyy"), it.getHeure().toString("HH:mm"),
                     QString::number(it.getIdZone()), nomZone, it.getGravite(), it.getStatut()));
        return;
    }
}

// ------------------------------------------------------------ Export PDF
void GestionInterventions::exporterPdf()
{
    const QString chemin = QFileDialog::getSaveFileName(this, tr("Exporter en PDF"),
                                                        "interventions.pdf", "PDF (*.pdf)");
    if (chemin.isEmpty())
        return;

    QString html = "<h2>Liste des interventions</h2>"
                   "<table border='1' cellspacing='0' cellpadding='4' width='100%'>"
                   "<tr bgcolor='#1f2d3d'>"
                   "<th><font color='white'>ID</font></th><th><font color='white'>Type</font></th>"
                   "<th><font color='white'>Adresse</font></th><th><font color='white'>Date et heure</font></th>"
                   "<th><font color='white'>Zone</font></th><th><font color='white'>Gravité</font></th>"
                   "<th><font color='white'>Statut</font></th></tr>";
    for (int i = 0; i < m_liste.size(); ++i) {
        const Intervention &it = m_liste[i];
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td><td>%7</td></tr>")
                    .arg(it.getId())
                    .arg(it.getType().toHtmlEscaped(), it.getAdresse().toHtmlEscaped(),
                         it.getDate().toString("dd/MM/yyyy") + " " + it.getHeure().toString("HH:mm"))
                    .arg(it.getIdZone())
                    .arg(it.getGravite().toHtmlEscaped(), it.getStatut().toHtmlEscaped());
    }
    html += "</table>";

    QPdfWriter writer(chemin);
    QTextDocument doc;
    doc.setHtml(html);
    doc.print(&writer);
    QMessageBox::information(this, tr("Exporter PDF"), tr("Fichier enregistré :\n%1").arg(chemin));
}

// ------------------------------------------------------------ Tris
void GestionInterventions::trierParDate()
{
    m_tri = TriDate;
    rafraichir();
}

void GestionInterventions::trierParGravite()
{
    m_tri = TriGravite;
    rafraichir();
}

// ------------------------------------------------------------ Statistiques
void GestionInterventions::afficherStats()
{
    const QVector<Intervention> toutes = Intervention::afficher();
    QString texte = tr("Total : %1 intervention(s)\n\nPar statut :\n").arg(toutes.size());
    const QStringList statuts = listeStatuts();
    for (int s = 0; s < statuts.size(); ++s) {
        int n = 0;
        for (int i = 0; i < toutes.size(); ++i)
            if (toutes[i].getStatut() == statuts[s]) ++n;
        texte += QString("   %1 : %2\n").arg(statuts[s]).arg(n);
    }
    texte += tr("\nPar gravité :\n");
    const QStringList gravites = listeGravites();
    for (int g = 0; g < gravites.size(); ++g) {
        int n = 0;
        for (int i = 0; i < toutes.size(); ++i)
            if (toutes[i].getGravite() == gravites[g]) ++n;
        texte += QString("   %1 : %2\n").arg(gravites[g]).arg(n);
    }
    QMessageBox::information(this, tr("Statistiques"), texte);
}

void GestionInterventions::fonctionnaliteAVenir()
{
    QMessageBox::information(this, tr("Interventions"), tr("Fonctionnalité à venir."));
}
