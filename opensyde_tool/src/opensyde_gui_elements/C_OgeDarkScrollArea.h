//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom scroll area with dark mode support (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEDARKSCROLLAREA_H
#define C_OGEDARKSCROLLAREA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QScrollArea>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeDarkScrollArea :
   public QScrollArea
{
   Q_OBJECT

public:
   C_OgeDarkScrollArea(QWidget * const opc_Parent = NULL);

private:
   void m_ScrollBarRangeChangedVer(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
   void m_ScrollBarRangeChangedHor(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
