#pragma once

#include <unordered_map>
#include <string>

#include "Position.hpp"

namespace owl
{
	/**
	 *  Datenstruktur zur Überprüfung der Stellungswiederholung. Diese verwendet ein Dictionary-Datenstruktur.
	 * Diese besteht aus Positions-Schlüssel (UINT64-Hash aus allen Zellen des Spielfelds)
	 * Jeder Schlüssel verweist auf die Anzahl der Wiederholungen der korrespondierenden Schachstellung.
	 */
	class RepitionMap
	{
	public:
		/**
		 * Neu Spielposition zur Repition Map hinzufügen. Wird für die dreimalige Stellungswiederholung verwendet.
		 * 
		 * \param position Die jeweilige Stellung
		 */
		VOID addPosition(const Position& position);
		/**
		 * Ist die Position bereits blockiert.
		 * 
		 * \param position Die zu überprüfende Stellung
		 * \return Ist die Position blockiert?
		 */
		BOOL isPositionAlreadyLocked(const Position& position) const;

		/**
		 * Hash-Funktions-Struct für Positionen
		 */
		struct HashFunction
		{
			/**
			 * Hash-Funktion für Positionen
			 * 
			 * \param position Die Stellung der gehasht werden soll
			 * \return Der Hashwert
			 */
			UINT64 operator()(const Position& position) const
			{
				UINT64 rows[ROWS];

				for (auto y = FIRST_ROW_INDEX; y < ROWS; y++)
				{
					std::string pos_str = "";
					for (auto x = FIRST_COLUMN_INDEX; x < COLUMNS; x++)
					{
						pos_str += position[y][x];
					}

					rows[y] = std::hash<std::string>()(pos_str);
				}
				
				UINT64 result = 0;
				for (auto row : rows)
				{
					result ^= row;
				}

				return result;
			}
		};
	private:
		mutable std::unordered_map<Position, INT32, HashFunction> m_data; // Hash-Map für die Stellungswiederholungsüberprüfung

	};
}