#pragma once

#include <unordered_map>
#include <string>

#include "Position.hpp"

namespace owl
{
	/// <summary>
	/// Datenstruktur zur Überprüfung der Stellungswiederholung. Diese verwendet ein Dictionary-Datenstruktur.
	/// - Diese besteht aus String-Schlüssel (64-CHAR Feldinformation)
	/// - Jeder Schlüssel verweist auf die Anzahl der Wiederholungen der korrespondierenden Schachstellung.
	/// </summary>
	class RepitionMap
	{
	public:
		// TODO: lange fen-form durch einfachen 64-CHAR-String ersetzen
		/// <summary>
		/// Neu Spielposition zur Repition Map hinzufügen. Wird für die dreimalige Stellungswiederholung verwendet.
		/// </summary>
		/// <param name="position:">Die jeweilige Stellung</param>
		VOID addPosition(const Position& position);

		BOOL isPositionAlreadyLocked(const Position& position) const;

		struct HashFunction
		{
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

		/// <summary>
		/// Stellungswiederholung-Dictionary
		/// string: Zusammensetzung aus 64 Felder bzw. Characters
		/// INT32: Die Anzahl der sich wiederholenden Stellung
		/// </summary>
		mutable std::unordered_map<Position, INT32, HashFunction> m_data;

	};
}