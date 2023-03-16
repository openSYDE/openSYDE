//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view node user settings (implementation)

   Made for all node specific system view user settings where the identifier for a node is the node name.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_UsSystemViewNode.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;

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
   this->mc_SectionsExpanded.push_back(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get expanded flags of sections

   \return
   vector of expanded flags (true: expanded, false: collapsed)
*/
//----------------------------------------------------------------------------------------------------------------------
const QVector<bool> & C_UsSystemViewNode::GetSectionsExpanded(void) const
{
   return this->mc_SectionsExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set expanded flags of sections

   \param[in]  orc_SectionsExpanded    Last known expanded flags of sections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemViewNode::SetSectionsExpanded(const QVector<bool> & orc_SectionsExpanded)
{
   this->mc_SectionsExpanded = orc_SectionsExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get update data rate history

   \return
   Update data rate history
*/
//----------------------------------------------------------------------------------------------------------------------
const QMap<uint32_t, float64_t> & C_UsSystemViewNode::GetUpdateDataRateHistory() const
{
   return this->mc_UpdateDataRateHistory;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add update data rate

   \param[in]  ou32_Checksum  Checksum
   \param[in]  of64_Value     Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsSystemViewNode::AddUpdateDataRate(const uint32_t ou32_Checksum, const float64_t of64_Value)
{
   this->mc_UpdateDataRateHistory.insert(ou32_Checksum, of64_Value);
}
