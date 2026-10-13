#include "integratedwindow.h"

#include "vehiclewindow.h"
#include "equipmentwindow.h"
#include "gestioninterventions.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

IntegratedWindow::IntegratedWindow(QWidget *parent)
    : QMainWindow(parent), pages(new QStackedWidget(this))
{
    auto *central = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *sidebar = new QFrame(central);
    sidebar->setObjectName("integrationSidebar");
    sidebar->setFixedWidth(235);
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(16, 22, 16, 18);
    sidebarLayout->setSpacing(10);

    auto *brand = new QLabel("FIRE STATION", sidebar);
    brand->setObjectName("integrationBrand");
    sidebarLayout->addWidget(brand);

    auto *subtitle = new QLabel("Gestion intégrée", sidebar);
    subtitle->setObjectName("integrationSubtitle");
    sidebarLayout->addWidget(subtitle);
    sidebarLayout->addSpacing(22);

    const QStringList labels = {"🚒  Véhicules", "🧰  Équipements", "🚨  Interventions"};
    for (int index = 0; index < labels.size(); ++index) {
        auto *button = new QPushButton(labels[index], sidebar);
        button->setCheckable(true);
        button->setProperty("pageIndex", index);
        sidebarLayout->addWidget(button);
        connect(button, &QPushButton::clicked, this, [this, sidebar, button, index]() {
            pages->setCurrentIndex(index);
            for (QPushButton *navButton : sidebar->findChildren<QPushButton *>())
                navButton->setChecked(navButton == button);
        });
    }
    sidebarLayout->addStretch(1);

    auto *vehicles = new MainWindow(pages);
    vehicles->setWindowFlags(Qt::Widget);
    pages->addWidget(vehicles);

    auto *equipment = new EquipmentWindow(pages);
    equipment->setWindowFlags(Qt::Widget);
    pages->addWidget(equipment);

    pages->addWidget(new GestionInterventions(pages));

    rootLayout->addWidget(sidebar);
    rootLayout->addWidget(pages, 1);
    setCentralWidget(central);
    setWindowTitle("Fire Station — Gestion intégrée");
    setWindowIcon(QIcon(":/station/images/logo_icone.png"));
    setStyleSheet(R"(
        #integrationSidebar { background: #641014; }
        #integrationBrand { color: white; font-size: 18px; font-weight: 700; }
        #integrationSubtitle { color: #f4d9da; font-size: 13px; }
        #integrationSidebar QPushButton {
            color: white; text-align: left; border: 0; border-radius: 7px;
            background: transparent; padding: 12px 10px; min-height: 24px;
        }
        #integrationSidebar QPushButton:hover,
        #integrationSidebar QPushButton:checked { background: #d71920; }
    )");

    const auto buttons = sidebar->findChildren<QPushButton *>();
    if (!buttons.isEmpty()) buttons.first()->setChecked(true);
}
