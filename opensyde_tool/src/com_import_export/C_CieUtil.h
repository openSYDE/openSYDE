//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class with functions for comm import export (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.05.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CIEUTIL_H
#define C_CIEUTIL_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "stwtypes.h"
#include "C_CieConverter.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CieUtil
{
public:
   C_CieUtil(void);

   static stw_types::sint32 h_ImportFile(const stw_types::uint32 ou32_NodeIndex,
                                         const stw_types::uint32 ou32_DataPoolIndex,
                                         const stw_types::uint32 ou32_InterfaceIndex, QWidget * const opc_Parent);
   static stw_types::sint32 h_ExportFile(
      const stw_opensyde_gui_logic::C_CieConverter::C_CIECommDefinition & orc_CommDef, QWidget * const opc_Parent,
      const stw_types::uint32 ou32_NumOfNodes, const stw_types::uint32 ou32_NumOfMessages,
      const stw_types::uint32 ou32_NumOfSignals);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
