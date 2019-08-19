//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool lists tree drawing delegate (implementation)

   Node data pool lists tree drawing delegate

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_SdNdeDataPoolListsTreeDelegate.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDataPoolListsTreeDelegate::C_SdNdeDataPoolListsTreeDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mu32_NodeIndex(0UL),
   mu32_DataPoolIndex(0UL),
   msn_MaximumHeight(500)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new data pool

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeDelegate::SetDataPool(const stw_types::uint32 ou32_NodeIndex,
                                                   const stw_types::uint32 ou32_DataPoolIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_DataPoolIndex = ou32_DataPoolIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Size hint of each index

   \param[in] orc_Option Style options
   \param[in] orc_Index  Index

   \return
   Item size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SdNdeDataPoolListsTreeDelegate::sizeHint(const QStyleOptionViewItem & orc_Option,
                                                 const QModelIndex & orc_Index) const
{
   QSize c_Retval = QStyledItemDelegate::sizeHint(orc_Option, orc_Index);

   if (orc_Index.isValid() == true)
   {
      if (orc_Index.parent().isValid() == true)
      {
         //dynamic
         const stw_types::sint32 s32_Height = C_SdNdeDataPoolUtil::h_GetTableSize(this->mu32_NodeIndex,
                                                                                  this->mu32_DataPoolIndex,
                                                                                  static_cast<uint32>(orc_Index.parent()
                                                                                                      .row()),
                                                                                  this->msn_MaximumHeight);
         c_Retval.setHeight(s32_Height);
      }
      else
      {
         //Header
         //static
         c_Retval.setHeight(66);
      }
   }
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the maximum height for the calculation of the table size in sizeHint

   \param[in]     sn_Height       Maximum height of entire tree widget

   \created     20.02.2019  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListsTreeDelegate::SetMaximumHeight(const sintn sn_Height)
{
   this->msn_MaximumHeight = sn_Height;
}
