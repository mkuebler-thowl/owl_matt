#pragma once

#include <map>
#include <string>

#include "Position.hpp"

namespace matt
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
		void addPosition(Position& position);
	private:
		/// <summary>
		/// Stellungswiederholung-Dictionary
		/// string: Zusammensetzung aus 64 Felder bzw. Characters
		/// int: Die Anzahl der sich wiederholenden Stellung
		/// </summary>
		std::map<std::string, int> m_data;
	};
}