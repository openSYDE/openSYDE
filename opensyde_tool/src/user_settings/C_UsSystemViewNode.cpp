//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view node user settings (implementation)

   Made for all node specific system view user settings where the identifier for a node is the node name.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_UsSystemViewNode.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsSystemViewNode::C_UsSystemViewNode(void)
{
   mc_SectionsExpanded[mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK] = true;
   mc_SectionsExpanded[mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_FILE] = true;
   mc_SectionsExpanded[mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET] = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get expanded flags of sections

   \return
   vector of expanded flags (true: expanded, false: collapsed)
*/
//----------------------------------------------------------------------------------------------------------------------
const QMap<uint32, bool> & C_UsSystemViewNode::GetSectionsExpanded(void) const
{
   return this->mc_SectionsExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set expanded flags of sections

   \param[in] orc_SectionsExpanded  Last known expanded flags of sections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemViewNode::SetSectionsExpanded(const QMap<uint32, bool> & orc_SectionsExpanded)
{
   this->mc_SectionsExpanded = orc_SectionsExpanded;
}
