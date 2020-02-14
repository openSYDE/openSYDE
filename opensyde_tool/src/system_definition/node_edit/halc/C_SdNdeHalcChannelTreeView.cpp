//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channels overview.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeHalcChannelTreeView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcChannelTreeView::C_SdNdeHalcChannelTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent)
{
   this->setModel(&this->mc_Model);
   this->setHeaderHidden(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward node index to model.

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::SetNode(const stw_types::uint32 ou32_NodeIndex)
{
   this->mc_Model.SetNode(ou32_NodeIndex);
   this->expandAll(); // TODO save and restore expanded domains in user settings
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update texts of specified channel.

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::UpdateChannelText(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex)
{
   this->mc_Model.UpdateChannelText(ou32_DomainIndex, ou32_ChannelIndex);
   this->expandAll(); // TODO remove when update does not only call SetNode
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear complete model.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelTreeView::Clear(void)
{
   this->mc_Model.Clear();
}
