# Fire Station — livrable S5

L'application regroupe les pages Qt des véhicules, équipements et interventions dans un menu latéral et un `QStackedWidget`. La fenêtre de connexion permet de choisir SQLite pour la démonstration locale ou Oracle via ODBC.

## Ouvrir et lancer

1. Ouvrir `FireStation_Integration.pro` avec Qt Creator.
2. Sélectionner un kit Qt 6 avec les modules Widgets, SQL, Charts et PrintSupport.
3. Compiler puis lancer. Choisir SQLite pour utiliser le fichier local `safestation.db`.
4. Sur Oracle XE 11g, choisir Oracle et saisir le pilote `Oracle in XE`, le service `XE`, votre nom utilisateur et votre mot de passe Oracle. L'alias `XE` existe dans le fichier `tnsnames.ora` de l'installation détectée sur ce poste. Le mot de passe est demandé à chaque lancement.
5. Le bouton « Tester et continuer » vérifie la connexion avec `SELECT 1 FROM DUAL`. Le mot de passe n'est pas enregistré ; il faut le ressaisir au prochain lancement.

## Schémas et cahier S5

- [`docs/modeles_donnees.md`](docs/modeles_donnees.md) : six modules, modèle Entité/Association et modèle relationnel.
- [`database/oracle_schema.sql`](database/oracle_schema.sql) : modèle physique Oracle XE 11g avec clés, contraintes, séquences, triggers et index pour toutes les tables.
- Le prototype Qt contient actuellement les interfaces Véhicules, Équipements et Interventions. Les modules Employés, Zones, Campagnes et Planning sont spécifiés dans le schéma et le modèle, mais leurs écrans CRUD restent à réaliser.

## Remarques

- La connexion SQLite crée automatiquement la table `EQUIPEMENT` utilisée par la page d'équipements.
- Avec Oracle, le prototype crée cette table et sa séquence/son trigger si elle est absente. Pour créer toutes les tables communes, connecter SQL Developer à `XE`, ouvrir `database/oracle_schema.sql`, puis lancer **Exécuter le script (F5)** dans votre schéma.
- Ne pas versionner les mots de passe. Les identifiants de connexion sont saisis dans l'application ; seuls le mode, le nom du pilote, le service et le nom utilisateur peuvent être mémorisés localement par Qt.
