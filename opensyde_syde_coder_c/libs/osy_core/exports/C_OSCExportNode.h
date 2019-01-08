//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Export code of a openSYDE node (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCEXPORTNODE_H
#define C_OSCEXPORTNODE_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "C_OSCNode.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
///Code exporter for server code: data pools, comm configuration, DPD and DPH initialization
class C_OSCExportNode
{
public:
   C_OSCExportNode(void);

   static stw_types::sint32 h_CreateSourceCode(const C_OSCNode & orc_Node,
                                               const stw_types::uint16 ou16_ApplicationIndex,
                                               const stw_scl::C_SCLString & orc_Path,
                                               std::vector<stw_scl::C_SCLString> & orc_Files);

private:
   static stw_types::sint32 mh_CheckPrerequisites(const C_OSCNode & orc_Node);
   static stw_types::sint32 mh_GetAdaptedComDataPool(const C_OSCNode & orc_Node,
                                                     const stw_types::uint32 ou32_DataPoolIndex,
                                                     C_OSCNodeDataPool & orc_DataPool);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
