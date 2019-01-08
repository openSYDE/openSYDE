//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Node update data (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.12.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISVNODEUPDATE_H
#define C_PUISVNODEUPDATE_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include <QString>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSvNodeUpdate
{
public:
   C_PuiSvNodeUpdate(const bool oq_FileBased);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   const std::vector<QString> & GetApplicationPaths(void) const;
   void SetApplicationPaths(const std::vector<QString> & orc_Value);

   void OnSyncNodeApplicationAdded(const stw_types::uint32 ou32_ApplicationIndex);
   void OnSyncNodeApplicationMoved(const stw_types::uint32 ou32_ApplicationSourceIndex,
                                   const stw_types::uint32 ou32_ApplicationTargetIndex);
   void OnSyncNodeApplicationAboutToBeDeleted(const stw_types::uint32 ou32_ApplicationIndex);

   stw_types::uint32 u32_NodeUpdatePosition; // Position of node in update sequence

private:
   bool mq_FileBased;
   std::vector<QString> mc_ApplicationPaths;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
