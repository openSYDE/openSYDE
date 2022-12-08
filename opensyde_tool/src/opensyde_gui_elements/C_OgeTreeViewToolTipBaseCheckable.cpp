//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view of code generation items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwerrors.hpp"

#include "TglUtils.hpp"
#include <QScrollBar>

#include "C_OgeTreeViewToolTipBaseCheckable.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::tgl;

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
C_OgeTreeViewToolTipBaseCheckable::C_OgeTreeViewToolTipBaseCheckable(QWidget * const opc_Parent) :
   C_OgeTreeViewToolTipBase(opc_Parent),
   mpc_Model(NULL)
{
   this->setItemDelegate(&this->mc_Delegate);

   this->setHeaderHidden(true);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}
C_OgeTreeViewToolTipBaseCheckable::~C_OgeTreeViewToolTipBaseCheckable(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialization of the view

   \param[in]  opc_Model   Model
   \param[in]  orc_ElementIndices   Indices of element in tree
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeViewToolTipBaseCheckable::Init(C_TblTreeModelCheckable * const opc_Model,
                                             const std::vector<uint32_t> & orc_ElementIndices)
{
   if (opc_Model != NULL)
   {
      this->mpc_Model = opc_Model;
      this->setModel(mpc_Model);
      tgl_assert(mpc_Model->Init(orc_ElementIndices) == C_NO_ERR);
      const uint32_t u32_CheckedElements = mpc_Model->GetCheckedItemCount();
      this->expandAll();
      connect(mpc_Model, &C_TblTreeModelCheckable::dataChanged,
              this, &C_OgeTreeViewToolTipBaseCheckable::m_ChangedData);
      Q_EMIT (this->SigSelectionChanged(u32_CheckedElements));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the checked items tree items

   \param[in]  opc_Model  related model to this view
   \param[out]  orc_ElementIndices         indices of element in tree
   \param[out]  orc_ChildIndicesPerElement   Vector of vectors of child indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeViewToolTipBaseCheckable::GetCheckedItems(const C_TblTreeModelCheckable * const opc_Model,
                                                        std::vector<uint32_t> & orc_ElementIndices,
                                                        std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerElement)
const
{
   if (opc_Model != NULL)
   {
      opc_Model->GetCheckedItems(orc_ElementIndices, orc_ChildIndicesPerElement);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of model changed data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeViewToolTipBaseCheckable::m_ChangedData()
{
   if (mpc_Model != NULL)
   {
      Q_EMIT (this->SigSelectionChanged(mpc_Model->GetCheckedItemCount()));
   }
}
