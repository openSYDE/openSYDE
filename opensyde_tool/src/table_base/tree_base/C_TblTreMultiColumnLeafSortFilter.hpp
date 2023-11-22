//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic filter for a multi column model displaying leaf items as result
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTREMULTICOLUMNLEAFSORTFILTER_HPP
#define C_TBLTREMULTICOLUMNLEAFSORTFILTER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblTreDataElementSortFilter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreMultiColumnLeafSortFilter :
   public C_TblTreDataElementSortFilter
{
public:
   C_TblTreMultiColumnLeafSortFilter();

protected:
   bool m_Contains(const QModelIndex & orc_Index) const override;
   bool lessThan(const QModelIndex & orc_SourceLeft, const QModelIndex & orc_SourceRight) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
