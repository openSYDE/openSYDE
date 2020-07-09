//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for HALC configuration import items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>

#include "C_SdNdeHalcConfigImportView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;

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
   this->setModel(&this->mc_Model);

   this->setHeaderHidden(true);

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

   \retval   C_NO_ERR  No errors, model is initialized
   \retval   C_NOACT   No domains does match (Id does not match), nothing to import
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcConfigImportView::Init(const C_OSCHalcConfig & orc_Config,
                                         const C_OSCHalcConfigStandalone & orc_ImportConfig)
{
   const sint32 s32_Return = this->mc_Model.Init(orc_Config, orc_ImportConfig);

   this->expandAll();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapts the selected elements of the HALC configuration set by Init function and returns the new config

   \param[out]    orc_AdaptedConfig   Adapted HALC configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcConfigImportView::GetAdaptedConfiguration(C_OSCHalcConfig & orc_AdaptedConfig)
{
   return this->mc_Model.GetAdaptedConfiguration(orc_AdaptedConfig);
}
