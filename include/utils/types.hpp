/**
 * @file Types.hpp
 * @brief Définition des types et énumérations communs au projet Yalta Chess
 */

#pragma once

#include <string>

namespace Yalta {

    /**
     * @brief Couleur d'un joueur ou d'une case
     */
    enum class Couleur {
        BLANC,
        NOIR,
        GRIS,      // Zone neutre / troisième joueur
        AUCUNE     // Case non attribuée
    };

    /**
     * @brief Type d'une pièce d'échecs
     */
    enum class TypePiece {
        PION,
        TOUR,
        FOU,
        CAVALIER,
        REINE,
        ROI
    };

    /**
     * @brief Événements du jeu pour le pattern Observer
     */
    enum class EvenementJeu {
        COUP_JOUE,
        ECHEC,
        ECHEC_ET_MAT,
        PARTIE_NULLE,
        PIECE_CAPTUREE,
        PROMOTION,
        JOUEUR_ELIMINE
    };

    /**
     * @brief État global d'une partie
     */
    enum class EtatPartie {
        EN_COURS,
        VICTOIRE_BLANC,
        VICTOIRE_NOIR,
        VICTOIRE_GRIS,
        NULLE
    };

    /**
     * @brief Représente un coup valide (pour l'IA et la validation)
     */
    struct CoupValide {
        int caseDepart;
        int caseArrivee;
        bool estCapture;
        
        CoupValide(int depart, int arrivee, bool capture = false)
            : caseDepart(depart), caseArrivee(arrivee), estCapture(capture) {}
    };

    /**
     * @brief Coordonnées 2D pour l'affichage SFML
     */
    struct CoordonneesEcran {
        float x;
        float y;
        
        CoordonneesEcran(float x_ = 0.f, float y_ = 0.f) : x(x_), y(y_) {}
    };

} 