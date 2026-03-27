/**
 * @file Piece.hpp
 * @brief Classe abstraite représentant une pièce d'échecs
 * 
 * Pattern Strategy : chaque pièce délègue le calcul de ses mouvements
 * à une stratégie IMouvement injectée au constructeur.
 */

#pragma once

#include "../utils/types.hpp"
#include <string>
#include <vector>

namespace Yalta {

    class Case;  
    class Plateau;
    class IMouvement;  

    class Piece {
    protected:
        Couleur m_couleur;
        Case* m_position;
        IMouvement* m_strategieMouvement;  
        bool m_aDejaJoue;

    public:
        Piece(Couleur couleur, Case* position, IMouvement* strategie);
        virtual ~Piece() = default;

       
        virtual std::string getNom() const = 0;
        virtual int getValeur() const = 0;

       
        std::vector<Case*> getCoupsPossibles(const Plateau& plateau) const;
        void deplacer(Case* nouvelleCase);
        
        // Getters
        Couleur getCouleur() const { return m_couleur; }
        Case* getPosition() const { return m_position; }
        bool aDejaJoue() const { return m_aDejaJoue; }
    };

} 