#include "model/Plateau.hpp"
// #include "Joueur.hpp"
// #include "Piece.hpp"
#include <iostream>

namespace Yalta {

    Plateau::Plateau()
        : m_tourCourant(Couleur::BLANC)
        , m_etatPartie(EtatPartie::EN_COURS)
    {
        // 1. Créer les 96 cases (positions écran à définir selon ton rendu hexagonal)
        for (int i = 0; i < 96; ++i) {
            // Coordonnées écran fictives — à adapter selon ton affichage hexagonal
            float x = (i % 12) * 50.f;
            float y = (i / 12) * 50.f;
            Couleur zone = (i < 32) ? Couleur::BLANC : 
                          (i < 64) ? Couleur::NOIR : Couleur::GRIS;
            
            m_cases[i] = new Case(i, CoordonneesEcran(x, y), zone);
        }

        // 2. Construire le graphe de connexions (CŒUR DU PROJET)
        initialiserGrapheCases();

        // 3. Placer les pièces en position de départ
        initialiserPieces();
    }

    Plateau::~Plateau() {
        for (Case* c : m_cases) {
            delete c;  // Les pièces sont gérées par les Joueur/PieceManager
        }
    }

    void Plateau::initialiserGrapheCases() {
        /**
         * @note C'est ici que tu définis la topologie du plateau Yalta.
         * Exemple simplifié : chaque case a jusqu'à 6 voisins hexagonaux.
         * 
         * Pour le vrai plateau Yalta, tu devras mapper les 96 cases selon
         * leur disposition réelle (zones triangulaires + centre partagé).
         * 
         * Astuce : pré-calcule les voisinages dans un fichier de config
         * ou une fonction dédiée pour ne pas alourdir le constructeur.
         */
        
        // Exemple générique (à adapter) :
        for (int i = 0; i < 96; ++i) {
            Case* current = m_cases[i];
            
            // Voisins horizontaux (à adapter selon la géométrie réelle)
            if (i % 12 != 0) current->ajouterVoisin(m_cases[i - 1]);      // Gauche
            if (i % 12 != 11) current->ajouterVoisin(m_cases[i + 1]);     // Droite
            
            // Voisins verticaux (hexagonal : décalage selon la ligne)
            if (i >= 12) current->ajouterVoisin(m_cases[i - 12]);         // Haut
            if (i < 84) current->ajouterVoisin(m_cases[i + 12]);          // Bas
            
            // Voisins diagonaux hexagonaux (à affiner selon ta grille)
            // ...
        }
    }

    void Plateau::initialiserPieces() {
        /**
         * @note Position de départ selon les règles Yalta :
         * - 16 pièces par joueur dans sa zone triangulaire
         * - Ordre : pions devant, pièces majeures derrière
         * 
         * À implémenter avec la Factory de pièces (pattern à venir)
         */
        // TODO: Créer et placer les pièces avec PieceFactory
    }

    Case* Plateau::getCase(int id) const {
        if (id < 0 || id >= 96) return nullptr;
        return m_cases[id];
    }

    std::vector<CoupValide> Plateau::getCoupsPossibles(Couleur joueur) const {
        std::vector<CoupValide> coups;
        // TODO: Parcourir toutes les cases, pour chaque pièce du joueur,
        // appeler piece->getCoupsPossibles(*this) et aggregator les résultats
        return coups;
    }

    bool Plateau::deplacerPiece(int fromId, int toId) {
        Case* from = getCase(fromId);
        Case* to = getCase(toId);
        
        if (!from || !to || from->estVide()) return false;
        
        Piece* piece = from->getPiece();
        // TODO: Valider le coup via piece->peutAllerVers(*to, *this)
        
        // Exécution du déplacement
        to->setPiece(piece);
        from->setPiece(nullptr);
        
        // Notification via Observer (à connecter plus tard)
        // notifier(EvenementJeu::COUP_JOUE, {fromId, toId});
        
        return true;
    }

    bool Plateau::estEchec(int roiId) const {
        // TODO: Vérifier si le roi est menacé par une pièce adverse
        return false;
    }

    bool Plateau::estMat(int roiId) const {
        if (!estEchec(roiId)) return false;
        
        // TODO: Vérifier si aucun coup ne permet de sortir de l'échec
        return false;
    }

    void Plateau::tourSuivant() {
        // Cycle BLANC -> NOIR -> GRIS -> BLANC...
        switch (m_tourCourant) {
            case Couleur::BLANC: m_tourCourant = Couleur::NOIR; break;
            case Couleur::NOIR:  m_tourCourant = Couleur::GRIS; break;
            case Couleur::GRIS:  m_tourCourant = Couleur::BLANC; break;
            default: break;
        }
    }

} // namespace Yalta