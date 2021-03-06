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

#ifndef LIBREPCB_CMDPOLYGONSEGMENTEDIT_H
#define LIBREPCB_CMDPOLYGONSEGMENTEDIT_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include "../../undocommand.h"
#include "../../units/all_length_units.h"

/*****************************************************************************************
 *  Namespace / Forward Declarations
 ****************************************************************************************/
namespace librepcb {

class PolygonSegment;

/*****************************************************************************************
 *  Class CmdPolygonSegmentEdit
 ****************************************************************************************/

/**
 * @brief The CmdPolygonSegmentEdit class
 */
class CmdPolygonSegmentEdit final : public UndoCommand
{
    public:

        // Constructors / Destructor
        CmdPolygonSegmentEdit() = delete;
        CmdPolygonSegmentEdit(const CmdPolygonSegmentEdit& other) = delete;
        explicit CmdPolygonSegmentEdit(PolygonSegment& segment) noexcept;
        ~CmdPolygonSegmentEdit() noexcept;

        // Setters
        void setEndPos(const Point& pos, bool immediate) noexcept;
        void setDeltaToStartPos(const Point& deltaPos, bool immediate) noexcept;
        void rotate(const Angle& angle, const Point& center, bool immediate) noexcept;
        void setAngle(const Angle& angle, bool immediate) noexcept;

        // Operator Overloadings
        CmdPolygonSegmentEdit& operator=(const CmdPolygonSegmentEdit& rhs) = delete;


    private:

        // Private Methods

        /// @copydoc UndoCommand::performExecute()
        bool performExecute() override;

        /// @copydoc UndoCommand::performUndo()
        void performUndo() override;

        /// @copydoc UndoCommand::performRedo()
        void performRedo() override;


        // Private Member Variables

        // Attributes from the constructor
        PolygonSegment& mSegment;

        // General Attributes
        Point mOldEndPos;
        Point mNewEndPos;
        Angle mOldAngle;
        Angle mNewAngle;
};

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace librepcb

#endif // LIBREPCB_CMDPOLYGONSEGMENTEDIT_H
