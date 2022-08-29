#pragma once
#include "defines.hpp"
#include "Move.hpp"
#include <unordered_map>

namespace owl
{
	using MoveMap = std::unordered_map<Move, EVALUATION_VALUE, Move::HashFunction>;

	/**
	 * Result-Objekt für die Zugfindung mit dem Min-Max-Algorithmus
	 */
	class MinMaxResult
	{
	public:
		/**
		 * Füge einen neuen "besten" gefunden Zug hinzu bzw. ersetze ihn.
		 * 
		 * \param move Der neue Zug
		 * \param value Der aktuell neue "beste" Wert
		 * \param shouldMax Soll der "beste" Wert maximiert werden
		 */
		VOID insert(const Move& move, const EVALUATION_VALUE value, BOOL shouldMax) const;
		/**
		 * Gebe das Ergebnis zurück.
		 * 
		 * \return Ergebnis bestehend aus Zug und Wertung
		 */
		std::pair<Move, EVALUATION_VALUE> getResult() const;
		/**
		 * Ist die Zufallszugsliste leer?
		 * 
		 * \return Zufallszugsliste leer?
		 */
		BOOL empty() const;
		/**
		 * Leere die Zufallszugsliste und setzte den aktuell "besten" Wert zurück.
		 */
		VOID clear() const;
	private:
		/**
		 * Aktualisiere den "besten" Wert.
		 * 
		 * \param newValue der neue Wert
		 * \param shouldMax soll maximiert werden?
		 */
		void updateCurrentValue(const EVALUATION_VALUE newValue, BOOL shouldMax) const;

		mutable EVALUATION_VALUE m_currentBestValue = -INF; // der aktuell "beste" gefundene Wert aus einer Evaluierung
#if OWL_USE_RANDOM==true
	private:
		mutable MoveMap m_result; // Liste der Zufallszüge
	public:
		const MoveMap testGetResults() const; // Gib die Liste der Zufallszüge aus
#else
		mutable Move m_best; // Der aktuell "beste" Zug
#endif
	};
}
