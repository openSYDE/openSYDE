//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeHalcConfigTreeView.h"

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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigTreeView::C_SdNdeHalcConfigTreeView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent)
{
   this->setModel(&this->mc_Model);
   this->setItemDelegate(&this->mc_Delegate);
   this->setSelectionMode(QAbstractItemView::NoSelection);

   // TODO style
   this->header()->setMinimumSectionSize(200);
   this->header()->setDefaultSectionSize(200);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Forward node index to model.

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeView::SetNode(const stw_types::uint32 ou32_NodeIndex)
{
   this->mc_Model.SetNode(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Forward HALC channel and use case to model.

   \param[in] ou32_DomainIndex  HALC Domain index
   \param[in] ou32_ChannelIndex HALC Channel index
   \param[in] ou32_UseCaseIndex HALC Use case index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeView::SetHalcChannelUseCase(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                                      const uint32 ou32_UseCaseIndex)
{
   this->mc_Model.SetHalcChannelUseCase(ou32_DomainIndex, ou32_ChannelIndex, ou32_UseCaseIndex);
   this->expandAll(); // TODO decide if some/all/none tree items shall be expanded on use case change
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear complete model.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigTreeView::Clear(void)
{
   this->mc_Model.Clear();
}
