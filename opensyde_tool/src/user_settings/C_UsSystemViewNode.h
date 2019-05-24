//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view node user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USSYSTEMVIEWNODE_H
#define C_USSYSTEMVIEWNODE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsSystemViewNode
{
public:
   C_UsSystemViewNode(void);

   const QMap<stw_types::uint32, bool> & GetSectionsExpanded(void) const;
   void SetSectionsExpanded(const QMap<stw_types::uint32, bool> & orc_SectionsExpanded);

private:
   QMap<stw_types::uint32, bool> mc_SectionsExpanded;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
