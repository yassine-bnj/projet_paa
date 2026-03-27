/**
 * @file Plateau.hpp
 * @brief Représente le plateau hexagonal Yalta (96 cases)
 * 
 * Structure en graphe : chaque Case connaît ses voisines.
 * 
 */

#pragma once

#include "../utils/types.hpp"
#include "model/Case.hpp"
#include <array>
#include <vector>
#include <memory>

namespace Yalta {

    class Joueur;
    class Piece;

    class Plateau {
    private:
        // Tableau de pointeurs vers les 96 cases
        std::array<Case*, 96> m_cases;
        
        // Gestion des joueurs 
        std::vector<Joueur*> m_joueurs;
        
        // Tour de jeu actuel
        Couleur m_tourCourant;
        
        // État de la partie
        EtatPartie m_etatPartie;

        /**
         * @brief Initialise les connexions entre cases (construction du graphe)
         * @note Appelée une seule fois au constructeur
         */
        void initialiserGrapheCases();

        /**
         * @brief Place les pièces en position de départ selon les règles Yalta
         */
        void initialiserPieces();

        /**
         * @brief Vérifie si un roi est en échec
         * @param roi Pointeur vers le roi à tester
         * @return true si le roi est menacé
         */
        bool verifierEchec(const Piece* roi) const;

    public:
        /**
         * @brief Constructeur : initialise le plateau Yalta
         */
        Plateau();

        /**
         * @brief Destructeur : libère les cases (les pièces sont gérées ailleurs)
         */
        ~Plateau();

        // Accesseurs 
        
        /**
         * @brief Retourne une case par son ID
         * @param id Identifiant de la case (0-95)
         * @return Pointeur vers la case, ou nullptr si invalide
         */
        Case* getCase(int id) const;

        /**
         * @brief Retourne les coups légaux pour un joueur donné
         * @param joueur Couleur du joueur
         * @return Vecteur de coups valides
         */
        std::vector<CoupValide> getCoupsPossibles(Couleur joueur) const;

        //  Actions sur le jeu 

        /**
         * @brief Tente de déplacer une pièce d'une case à une autre
         * @param fromId ID de la case de départ
         * @param toId ID de la case d'arrivée
         * @return true si le déplacement est valide et effectué
         */
        bool deplacerPiece(int fromId, int toId);

        /**
         * @brief Vérifie si un roi est en échec
         * @param roiId ID de la case contenant le roi
         * @return true si échec détecté
         */
        bool estEchec(int roiId) const;

        /**
         * @brief Vérifie si un roi est mat (impossible de sortir de l'échec)
         * @param roiId ID de la case contenant le roi
         * @return true si échec et mat
         */
        bool estMat(int roiId) const;

        /**
         * @brief Passe au tour du joueur suivant
         */
        void tourSuivant();

        // Getters d'état 
        Couleur getTourCourant() const { return m_tourCourant; }
        EtatPartie getEtatPartie() const { return m_etatPartie; }
        const std::array<Case*, 96>& getCases() const { return m_cases; }
    };

} 