-- =====================================================================
--  Script Oracle 11g XE - module Interventions
--  A exécuter dans SQL Developer (connexion emna) avec F5
--  (« Exécuter le script »).
-- =====================================================================

-- ---------------------------------------------------------------------
-- PARTIE 1 : ZONE_COUVERTURE (d'après le MLD)
-- A EXECUTER SEULEMENT si la table n'existe pas encore chez toi
-- (si l'équipe a déjà un script commun, utilise le leur et saute cette partie).
-- ---------------------------------------------------------------------
CREATE TABLE ZONE_COUVERTURE (
  ID_ZONE            NUMBER PRIMARY KEY,
  NOM                VARCHAR2(50) NOT NULL,
  REGION             VARCHAR2(50),
  SUPERFICIE         NUMBER,
  POPULATION         NUMBER,
  NIVEAU_RISQUE      VARCHAR2(20),
  TEMPS_INTERVENTION NUMBER
);

INSERT INTO ZONE_COUVERTURE VALUES (1, 'Bab El Bhar',  'Tunis',       3.2,  45000, 'Moyen', 7);
INSERT INTO ZONE_COUVERTURE VALUES (2, 'La Marsa',     'Tunis Nord',  25.0, 92000, 'Eleve', 12);
INSERT INTO ZONE_COUVERTURE VALUES (3, 'Ariana Ville', 'Ariana',      18.5, 114000,'Eleve', 11);
INSERT INTO ZONE_COUVERTURE VALUES (4, 'Le Bardo',     'Tunis Ouest', 12.0, 73000, 'Moyen', 9);
COMMIT;

-- ---------------------------------------------------------------------
-- PARTIE 2 : INTERVENTIONS (d'après le MLD)
-- GRAVITE et STATUT ne sont pas dans le MLD actuel : à faire valider
-- par l'équipe / le tuteur. Sans elles, la page ne marche pas entièrement.
-- ---------------------------------------------------------------------
CREATE SEQUENCE SEQ_INTERVENTIONS START WITH 1 INCREMENT BY 1 NOCACHE;

CREATE TABLE INTERVENTIONS (
  ID_INTERVENTION    NUMBER PRIMARY KEY,
  TYPE_INTERVENTION  VARCHAR2(50)  NOT NULL,
  ADRESSE            VARCHAR2(150) NOT NULL,
  DATE_INTERVENTION  DATE          NOT NULL,
  HEURE              VARCHAR2(5)   NOT NULL,
  GRAVITE            VARCHAR2(20),
  STATUT             VARCHAR2(20),
  ID_ZONE            NUMBER        NOT NULL,
  CONSTRAINT FK_INTER_ZONE FOREIGN KEY (ID_ZONE)
    REFERENCES ZONE_COUVERTURE (ID_ZONE)
);

CREATE OR REPLACE TRIGGER TRG_INTERVENTIONS_ID
BEFORE INSERT ON INTERVENTIONS
FOR EACH ROW
WHEN (NEW.ID_INTERVENTION IS NULL)
BEGIN
  SELECT SEQ_INTERVENTIONS.NEXTVAL INTO :NEW.ID_INTERVENTION FROM DUAL;
END;
/
