#pragma once

#include "../utils/types.hpp"
#include <vector>

namespace Yalta {

    class Piece;  

    class Case {
    public:
        enum class Direction {
            NORD,
            SUD,
            EST,
            OUEST,
            NORD_EST,
            NORD_OUEST,
            SUD_EST,
            SUD_OUEST
        };

        struct Arc {
            Direction direction;
            Case* voisin;
        };

    private:
        int m_id;
        CoordonneesEcran m_positionEcran;
        Piece* m_piece;
        std::vector<Case*> m_voisins;
        std::vector<Arc> m_arcs;
        Couleur m_proprietaire;

    public:
        Case(int id, CoordonneesEcran pos, Couleur zone);
        void ajouterVoisin(Case* voisin);
        void ajouterArc(Direction direction, Case* voisin);
        const std::vector<Case*>& getVoisins() const;
        const std::vector<Arc>& getArcs() const;
        Case* getVoisin(Direction direction) const;
        bool estVide() const;
        void setPiece(Piece* piece);
        Piece* getPiece() const;
        Piece* retirerPiece();
        
        int getId() const { return m_id; }
        CoordonneesEcran getPositionEcran() const { return m_positionEcran; }
        Couleur getProprietaire() const { return m_proprietaire; }
        Couleur getZone() const { return m_proprietaire; }
        bool appartientA(Couleur couleur) const;
    };

} 