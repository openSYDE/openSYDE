//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view of code generation items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QScrollBar>

#include "C_SdCodeGenerationView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

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
C_SdCodeGenerationView::C_SdCodeGenerationView(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent)
{
   this->setModel(&this->mc_Model);
   this->setItemDelegate(&this->mc_Delegate);

   this->setHeaderHidden(true);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   connect(&this->mc_Model, &C_SdCodeGenerationModel::dataChanged,
           this, &C_SdCodeGenerationView::m_ChangedData);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdCodeGenerationView::~C_SdCodeGenerationView(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the view

   \param[in]  orc_NodeIndices   Node indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationView::Init(const std::vector<uint32> & orc_NodeIndices)
{
   uint32 u32_CheckedApps = this->mc_Model.Init(orc_NodeIndices);

   this->expandAll();
   Q_EMIT (this->SigSelectionChanged(u32_CheckedApps));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the checked items to code generation

   \param[out]  orc_NodeIndices         Node indices (ID)
   \param[out]  orc_AppIndicesPerNode   Vector of vectors of application indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationView::GetCheckedItems(std::vector<uint32> & orc_NodeIndices,
                                             std::vector<std::vector<uint32> > & orc_AppIndicesPerNode) const
{
   this->mc_Model.GetCheckedItems(orc_NodeIndices, orc_AppIndicesPerNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of model changed data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdCodeGenerationView::m_ChangedData(void)
{
   Q_EMIT (this->SigSelectionChanged(this->mc_Model.GetCheckedItemCount()));
}
