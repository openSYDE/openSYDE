//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for HALC configuration import items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>

#include "C_SdNdeHalcConfigImportView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigImportView::C_SdNdeHalcConfigImportView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent)
{
   this->C_SdNdeHalcConfigImportView::setModel(&this->mc_Model);
   this->setItemDelegate(&this->mc_Delegate);

   this->setHeaderHidden(true);
   this->setMouseTracking(true);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigImportView::~C_SdNdeHalcConfigImportView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the view

   \param[in]       orc_Config         HALC configuration of the current node
   \param[in]       orc_ImportConfig   Imported HALC configuration for updating

   \retval   C_NO_ERR   No errors, model is initialized
   \retval   C_NOACT    No domains does match (Id does not match), nothing to import
   \retval   C_CHECKSUM Configuration is not compatible
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeHalcConfigImportView::Init(const C_OscHalcConfig & orc_Config,
                                          const C_OscHalcConfigStandalone & orc_ImportConfig)
{
   const int32_t s32_Return = this->mc_Model.Init(orc_Config, orc_ImportConfig);

   this->expandAll();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapts the selected elements of the HALC configuration set by Init function and returns the new config

   \param[out]    orc_AdaptedConfig   Adapted HALC configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportView::GetAdaptedConfiguration(C_OscHalcConfig & orc_AdaptedConfig)
{
   return this->mc_Model.GetAdaptedConfiguration(orc_AdaptedConfig);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if selection of linked channels is valid

   \param[out]     orc_DomainIndices            Domain indices corresponding to orc_MissingChannelIndices
   \param[out]     orc_MissingChannelIndices    Indices of all linked channels that are not checked

   \retval  true     For all selected linked channels the link buddies are also selected
   \retval  false    There exist linked channels where one is selected and the other one not
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcConfigImportView::IsSelectionOfLinkedChannelsValid(std::vector<uint32_t> & orc_DomainIndices,
                                                                   std::vector<std::vector<uint32_t> > & orc_MissingChannelIndices)
{
   return this->mc_Model.IsSelectionOfLinkedChannelsValid(orc_DomainIndices, orc_MissingChannelIndices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channels

   \param[in]  orc_DomainIndices    Domain indices
   \param[in]  orc_ChannelIndices   Channel indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportView::CheckChannels(const std::vector<uint32_t> & orc_DomainIndices,
                                                const std::vector<std::vector<uint32_t> > & orc_ChannelIndices)
{
   return this->mc_Model.CheckChannels(orc_DomainIndices, orc_ChannelIndices);
}
