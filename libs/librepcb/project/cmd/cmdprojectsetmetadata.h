/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * http://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREPCB_PROJECT_CMDPROJECTSETMETADATA_H
#define LIBREPCB_PROJECT_CMDPROJECTSETMETADATA_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include <librepcb/common/undocommand.h>
#include <librepcb/common/attributes/attribute.h>

/*****************************************************************************************
 *  Namespace / Forward Declarations
 ****************************************************************************************/
namespace librepcb {
namespace project {

class Project;

/*****************************************************************************************
 *  Class CmdProjectSetMetadata
 ****************************************************************************************/

/**
 * @brief The CmdProjectSetMetadata class
 */
class CmdProjectSetMetadata final : public UndoCommand
{
    public:

        // Constructors / Destructor
        explicit CmdProjectSetMetadata(Project& project) noexcept;
        ~CmdProjectSetMetadata() noexcept;

        // Setters
        void setName(const QString& newName) noexcept;
        void setAuthor(const QString& newAuthor) noexcept;
        void setVersion(const QString& newVersion) noexcept;
        void setAttributes(const AttributeList& attributes) noexcept;


    private:

        // Private Methods

        /// @copydoc UndoCommand::performExecute()
        bool performExecute() override;

        /// @copydoc UndoCommand::performUndo()
        void performUndo() override;

        /// @copydoc UndoCommand::performRedo()
        void performRedo() override;


        // Private Member Variables

        // General
        Project& mProject;

        // Misc
        QString mOldName;
        QString mNewName;
        QString mOldAuthor;
        QString mNewAuthor;
        QString mOldVersion;
        QString mNewVersion;
        AttributeList mOldAttributes;
        AttributeList mNewAttributes;
};

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb

#endif // LIBREPCB_PROJECT_CMDPROJECTSETMETADATA_H
