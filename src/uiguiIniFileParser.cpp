/***************************************************************************
 *   Copyright (C) 2006-2008 by Thomas Schweitzer                          *
 *   thomas-schweitzer(at)arcor.de                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2.0 as   *
 *   published by the Free Software Foundation.                            *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program in the file LICENSE.GPL; if not, write to the *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "uiguiIniFileParser.h"

#include <QTextStream>

UiguiIniFileParser::UiguiIniFileParser(void)
{
    sections.clear();
    keyValueMap.clear();
    iniFileName = "";
}


UiguiIniFileParser::UiguiIniFileParser(const QString &iniFileName)
{
    UiguiIniFileParser::UiguiIniFileParser();
    this->iniFileName = iniFileName;
    parseIniFile();
}


UiguiIniFileParser::~UiguiIniFileParser(void)
{
}


/*!
    \brief Returns the group/section names in the same order as they occurr in the ini file as QStringList.
 */
QStringList UiguiIniFileParser::childGroups()
{
    QStringList sectionsStringList;

    for( unsigned int i = 0; i < sections.size(); i++ ) {
        sectionsStringList << sections[i];
    }  

    return sectionsStringList;
}


/*!
    \brief Returns the value of the defined \a keyName as QVariant.

    The \a keyName is assembled by a section name, a slash and the key name itself.
    For example if you wish to access the value of the following setting:
    <code>[NiceSection]</br>niceKeyName=2</code> you would have to call
    value("NiceSection/niceKeyName").
 */
QVariant UiguiIniFileParser::value(const QString &keyName)
{
    return keyValueMap.value(keyName, QVariant("error") );   
}


/*!
    \brief Parses the ini file and stores the key value pairs in the internal vectors \a keys and \a values.
 */
void UiguiIniFileParser::parseIniFile()
{
    QFile iniFile(iniFileName);

    if ( iniFile.open(QFile::ReadOnly) ) {
        // Clear the vectors holding the keys and values.
        sections.clear();
        keyValueMap.clear();

        QTextStream iniFileStream( &iniFile );
        QString line;
        QString currentSectionName = "";
        QString keyName = "";
        QString keyValueAsString = "";

        while ( !iniFileStream.atEnd() ) {
            line = iniFileStream.readLine().trimmed();
            
            // Test if the read line is a section name and if so remeber it.
            if ( line.startsWith("[") && line.endsWith("]") ) {
                currentSectionName = line.remove(0, 1);
                currentSectionName.chop(1);

                // Store the section name.
                sections.push_back( currentSectionName );
            }
            // Otherwise test whether the line has a assign char
            else if ( line.contains("=") ) {
                int indexOfFirstAssign = line.indexOf("=");
                keyName = line.left(indexOfFirstAssign);

                if ( !keyName.isEmpty() ) {
                    keyValueAsString = line.remove(0, indexOfFirstAssign+1);

                    // Prepend an eventually section name to the key name.
                    if ( !currentSectionName.isEmpty() ) {
                        keyName = currentSectionName + "/" + keyName;
                    }

                    // Store the key and value in the map.
                    keyValueMap.insert(keyName, keyValueAsString );
                }
            }
        }
    }
}