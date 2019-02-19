//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Database data structure (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.12.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMPRODATABASEDATA_H
#define C_CAMPRODATABASEDATA_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include <QList>

#include "stwtypes.h"
#include "C_OSCComMessageLogger.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_CamProDatabaseData
{
public:
   C_CamProDatabaseData(void);

   QString c_Name;
   bool q_Enabled;
   stw_types::sint32 s32_BusIndex;

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
