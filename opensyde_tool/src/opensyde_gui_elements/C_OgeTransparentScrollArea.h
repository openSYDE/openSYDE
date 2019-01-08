//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Transparent (except scroll bars) scroll area (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETRANSPARENTSCROLLAREA_H
#define C_OGETRANSPARENTSCROLLAREA_H

/* -- Includes ------------------------------------------------------------- */

#include <QScrollArea>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTransparentScrollArea :
   public QScrollArea
{
   Q_OBJECT

public:
   C_OgeTransparentScrollArea(QWidget * const opc_Parent = NULL);

   void DeactivateScrollbarResize(void);

private:
   void m_ScrollBarRangeChanged(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
