#pragma once

#include <unordered_map>
#include <string>

#include "Position.hpp"

namespace owl
{
	/// <summary>
	/// Datenstruktur zur Überprüfung der Stellungswiederholung. Diese verwendet ein Dictionary-Datenstruktur.
	/// - Diese besteht aus String-Schlüssel (64-char Feldinformation)
	/// - Jeder Schlüssel verweist auf die Anzahl der Wiederholungen der korrespondierenden Schachstellung.
	/// </summary>
	class RepitionMap
	{
	public:
		// TODO: lange fen-form durch einfachen 64-char-String ersetzen
		/// <summary>
		/// Neu Spielposition zur Repition Map hinzufügen. Wird für die dreimalige Stellungswiederholung verwendet.
		/// </summary>
		/// <param name="position:">Die jeweilige Stellung</param>
		void addPosition(const Position& position);

		bool isPositionAlreadyLocked(const Position& position) const;

		struct HashFunction
		{
			size_t operator()(const Position& position) const
			{
				size_t rows[ROWS];

				for (auto y = FIRSTROWINDEX; y < ROWS; y++)
				{
					std::string pos_str = "";
					for (auto x = FIRSTCOLINDEX; x < COLUMNS; x++)
					{
						pos_str += position[y][x];
					}

					rows[y] = std::hash<std::string>()(pos_str);
				}
				
				size_t result = 0;
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
		/// int: Die Anzahl der sich wiederholenden Stellung
		/// </summary>
		mutable std::unordered_map<Position, int, HashFunction> m_data;

	};
}