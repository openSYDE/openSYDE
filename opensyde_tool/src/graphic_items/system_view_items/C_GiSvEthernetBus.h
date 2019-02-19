//-----------------------------------------------------------------------------
/*!
   \file
   \brief       System view ethernet bus (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GISVETHERNETBUS_H
#define C_GISVETHERNETBUS_H

/* -- Includes ------------------------------------------------------------- */
#include "C_GiLiEthernetBus.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiSvEthernetBus :
   public C_GiLiEthernetBus
{
public:
   C_GiSvEthernetBus(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 & ors32_Index,
                     const stw_types::uint64 & oru64_ID, C_GiTextElementBus * const opc_TextElementName,
                     const std::vector<QPointF> * const opc_Points = NULL, QGraphicsItem * const opc_Parent = NULL);
   virtual void CheckBusForChanges(void) override;

private:
   stw_types::uint32 mu32_ViewIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
