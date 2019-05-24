//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief        Table view base class with extra scrollbar feature (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLVIEWSCROLL_H
#define C_TBLVIEWSCROLL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_TblViewToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblViewScroll :
   public C_TblViewToolTipBase
{
public:
   C_TblViewScroll(QWidget * const opc_Parent = NULL);

private:
   void m_ScrollBarRangeChangedVer(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
   void m_ScrollBarRangeChangedHor(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
