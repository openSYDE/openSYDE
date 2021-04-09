//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model base class providing an interface for common actions (as far as possible) (implementation)

   Table model base class providing an interface for common actions (as far as possible)

   * Current supported actions:
   ** Add new item
   ** Copy selected items
   ** Paste from clipboard
   ** Cut selected items
   ** Delete selected items
   ** Move selected items (one direction only!)

   This interface is currently only intended to be used with the rowCount reflecting the current item count
   (may be split if required, don't forget the actions! e.g. beginMoveRow endMoveRow)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_Uti.h"
#include "stwtypes.h"
#include "C_TblModelAction.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblModelAction::C_TblModelAction(QObject * const opc_Parent) :
   QAbstractTableModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get table row count

   Compare with file header description.

   \param[in] orc_Parent Parent

   \return
   Row count
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_TblModelAction::rowCount(const QModelIndex & orc_Parent) const
{
   sintn sn_Retval;

   if (orc_Parent.isValid() == false)
   {
      sn_Retval = this->m_GetSizeItems();
   }
   else
   {
      // Qt documentation: When implementing a table based model, rowCount() should return 0 when the parent is valid.
      sn_Retval = 0;
   }
   return sn_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle add new item action

   \param[in] orc_SelectedIndices Selected row indices (Expected: unique)

   \return
   Index of new item
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_TblModelAction::AddNewItem(const std::vector<uint32> & orc_SelectedIndices)
{
   const uint32 u32_Retval = this->m_AddNewItem(m_GetLastSelectedIndex(orc_SelectedIndices));

   Q_EMIT this->SigItemCountChanged(this->m_GetSizeItems());
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle paste items action

   \param[in] orc_SelectedIndices Selected row indices (Expected: unique)

   \return
   Indices of new items
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32> C_TblModelAction::PasteItems(const std::vector<uint32> & orc_SelectedIndices)
{
   const std::vector<uint32> c_Retval = this->m_PasteItems(m_GetLastSelectedIndex(orc_SelectedIndices));

   Q_EMIT this->SigItemCountChanged(this->m_GetSizeItems());
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle cut selected items action

   \param[in] orc_SelectedIndices Selected row indices (Expected: unique)

   \return
   Valid item index after last deleted item index (highest value), if any
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_TblModelAction::CutSelectedItems(const std::vector<uint32> & orc_SelectedIndices)
{
   uint32 u32_Retval;

   this->CopySelectedItems(orc_SelectedIndices);
   u32_Retval = this->DeleteSelectedItems(orc_SelectedIndices);
   Q_EMIT this->SigItemCountChanged(this->m_GetSizeItems());
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle delete selected items action

   \param[in] orc_SelectedIndices Selected row indices (Expected: unique)

   \return
   Valid item index after last deleted item index (highest value), if any
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_TblModelAction::DeleteSelectedItems(const std::vector<uint32> & orc_SelectedIndices)
{
   uint32 u32_Retval = 0UL;

   std::vector<std::vector<uint32> > c_ContiguousSections =
      C_Uti::h_GetContiguousSectionsAscending(orc_SelectedIndices);
   //Start deleting from back (easier to keep indices valid)
   for (uint32 u32_ItSection = c_ContiguousSections.size(); u32_ItSection > 0UL; --u32_ItSection)
   {
      const std::vector<uint32> & rc_Section =
         c_ContiguousSections[static_cast<std::vector<uint32>::size_type>(u32_ItSection - 1UL)];
      if (rc_Section.size() > 0UL)
      {
         const uint32 u32_FirstDeletedIndex = rc_Section[0UL];
         const uint32 u32_LastDeletedIndex =
            rc_Section[static_cast<std::vector<uint32>::size_type>(rc_Section.size() - 1UL)];
         this->m_BeginRemoveRows(u32_FirstDeletedIndex, u32_LastDeletedIndex);
         //Remove all items in the current section (from back to front -> easier to keep next index valid)
         for (uint32 u32_ItItem = rc_Section.size(); u32_ItItem > 0UL; --u32_ItItem)
         {
            //Only remove item if its index is in range (as its a public interface its probably better to check)
            if (rc_Section[static_cast<std::vector<uint32>::size_type>(u32_ItItem - 1UL)] < this->m_GetSizeItems())
            {
               this->m_DeleteItem(rc_Section[static_cast<std::vector<uint32>::size_type>(u32_ItItem - 1UL)]);
            }
         }
         this->m_EndRemoveRows(u32_FirstDeletedIndex, u32_LastDeletedIndex);
      }
   }
   //If there is at least one item in the input both of these operations should not fail
   if (c_ContiguousSections.size() > 0UL)
   {
      const std::vector<uint32> & rc_Section =
         c_ContiguousSections[static_cast<std::vector<std::vector<uint32> >::size_type>(c_ContiguousSections.size() -
                                                                                        1UL)];
      if (rc_Section.size() > 0UL)
      {
         u32_Retval = rc_Section[static_cast<std::vector<uint32>::size_type>(rc_Section.size() - 1UL)];
      }
   }
   //Signal number of items changed
   Q_EMIT this->SigItemCountChanged(this->m_GetSizeItems());
   if (this->m_GetSizeItems() > 0UL)
   {
      //Get best index to select after this action -> ideal: one item after last deleted item
      // (or the one before that if it was the end)
      u32_Retval = std::min(this->m_GetSizeItems() - 1UL, (u32_Retval - orc_SelectedIndices.size()) + 1UL);
   }
   else
   {
      //No proper default available here
      u32_Retval = 0UL;
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle move selected items action

   Warning 1: This function currently can only handle shifts by ONE up or down

   \param[in] orc_SelectedIndices Selected row indices (Expected: unique)
   \param[in] oq_Up               Flag to switch to move one step up or down
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblModelAction::MoveSelectedItems(const std::vector<uint32> & orc_SelectedIndices, const bool oq_Up)
{
   std::vector<uint32> c_TargetIndices = orc_SelectedIndices;
   std::vector<uint32> c_SelectedIndicesCopy = orc_SelectedIndices;
   std::vector<std::vector<uint32> > c_ContiguousSections;
   uint32 u32_TargetAccessIndex = 0UL;
   bool q_InBetween;
   //"Calculate" target indices based on source indices and specified direction
   if (oq_Up == true)
   {
      //Specify target indices: one step "up"
      for (uint32 u32_It = 0UL; u32_It < c_TargetIndices.size(); ++u32_It)
      {
         uint32 & ru32_CurItem = c_TargetIndices[u32_It];
         --ru32_CurItem;
      }
   }
   else
   {
      //Specify target indices: one step "down"
      for (uint32 u32_It = 0UL; u32_It < c_TargetIndices.size(); ++u32_It)
      {
         uint32 & ru32_CurItem = c_TargetIndices[u32_It];
         ++ru32_CurItem;
      }
   }
   //Check if move can be normally performed
   q_InBetween = this->CheckAllIndicesInRange(c_TargetIndices);

   //Step 1 sort (so the next step can assume the contiguous selection has the same order!
   C_Uti::h_SortIndicesAscendingAndSync(c_SelectedIndicesCopy, c_TargetIndices);

   //Step 2: get contiguous selection
   c_ContiguousSections = C_Uti::h_GetContiguousSectionsAscending(orc_SelectedIndices);

   //Step 3: move each contiguous section
   for (uint32 u32_ItSection = 0UL; u32_ItSection < c_ContiguousSections.size(); ++u32_ItSection)
   {
      bool q_Continue;
      //Check if the move can be performed without any considerations
      if (q_InBetween)
      {
         q_Continue = true;
      }
      else
      {
         //Special handling for first/last section if move up/down
         if (oq_Up)
         {
            if (u32_ItSection == 0UL)
            {
               const std::vector<uint32> & rc_Section = c_ContiguousSections[u32_ItSection];
               q_Continue = false;
               //This still has to happen! (even if we skip that section!)
               u32_TargetAccessIndex += rc_Section.size();
            }
            else
            {
               q_Continue = true;
            }
         }
         else
         {
            if (u32_ItSection == (c_ContiguousSections.size() - 1UL))
            {
               q_Continue = false;
            }
            else
            {
               q_Continue = true;
            }
         }
      }
      if (q_Continue)
      {
         const std::vector<uint32> & rc_Section = c_ContiguousSections[u32_ItSection];
         if (rc_Section.size() > 0UL)
         {
            uint32 u32_TargetIndex = c_TargetIndices[u32_TargetAccessIndex];
            uint32 u32_TargetIndexParam = c_TargetIndices[u32_TargetAccessIndex];
            //Special adaptation of target index for moving "down" (our & qt interface)
            if (u32_TargetIndex > rc_Section[0UL])
            {
               //Qt interface seems to insert the items before removing anything so the "new position" has to have an
               // bigger offset
               u32_TargetIndex += rc_Section.size();
               //We insert after removing so this has to be considered
               u32_TargetIndexParam = u32_TargetIndex - 1UL;
            }
            this->beginMoveRows(QModelIndex(), rc_Section[0UL],
                                rc_Section[static_cast<std::vector<uint32>::size_type>(rc_Section.size() - 1UL)],
                                QModelIndex(), u32_TargetIndex);
            this->m_MoveItems(rc_Section, u32_TargetIndexParam);
            this->endMoveRows();
            u32_TargetAccessIndex += rc_Section.size();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if all indices are inside of item size boundary (can be used for move operation target index check)

   \param[in] orc_Indices Indices to check

   \return
   True  All valid
   False At least one out of range
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblModelAction::CheckAllIndicesInRange(const std::vector<uint32> & orc_Indices) const
{
   bool q_Retval = true;

   for (uint32 u32_ItItem = 0UL; u32_ItItem < orc_Indices.size(); ++u32_ItItem)
   {
      if (orc_Indices[u32_ItItem] >= this->m_GetSizeItems())
      {
         q_Retval = false;
         break;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload table content (after change of data outside this classes responsibility)

   Warning: should only be used if type of change has unknown effect on complete table content
   -> if only one column or row changes use dataChanged instead
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblModelAction::ReloadAll(void)
{
   this->beginResetModel();
   this->endResetModel();
   Q_EMIT this->SigItemCountChanged(this->m_GetSizeItems());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger table signal for start of remove action

   \param[in] ou32_FirstIndex Lowest index of this section of removed items
   \param[in] ou32_LastIndex  Highest index of this section of removed items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblModelAction::m_BeginRemoveRows(const uint32 ou32_FirstIndex, const uint32 ou32_LastIndex)
{
   this->beginRemoveRows(QModelIndex(), ou32_FirstIndex, ou32_LastIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger table signal for end of remove action

   \param[in] ou32_FirstIndex Lowest index of this section of removed items
   \param[in] ou32_LastIndex  Highest index of this section of removed items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblModelAction::m_EndRemoveRows(const uint32 ou32_FirstIndex, const uint32 ou32_LastIndex)
{
   Q_UNUSED(ou32_FirstIndex)
   Q_UNUSED(ou32_LastIndex)
   this->endRemoveRows();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle hex value entry display & edit role

   \param[in] ou64_Value Value
   \param[in] osn_Role   Role

   \return
   Value as expected by generic interface
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_TblModelAction::mh_HandleHexValue(const uint64 ou64_Value, const sintn osn_Role)
{
   QVariant c_Retval;

   if (osn_Role == static_cast<sintn>(Qt::DisplayRole))
   {
      c_Retval = C_Uti::h_GetValueAsHex(ou64_Value);
   }
   else
   {
      c_Retval = ou64_Value;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility function to transform bool value into Qt::CheckState stored as QVariant

   \param[in] oq_Value Value to encode

   \return
   Qt::CheckState stored as QVariant
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_TblModelAction::mh_GetBoolAsCheckStateVariant(const bool oq_Value)
{
   QVariant c_Retval;

   if (oq_Value)
   {
      c_Retval = static_cast<sintn>(Qt::Checked);
   }
   else
   {
      c_Retval = static_cast<sintn>(Qt::Unchecked);
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility function to transform Qt::CheckState stored as QVariant into bool

   \param[in] orc_Value Qt::CheckState stored as QVariant

   \return
   Qt::CheckState stored as QVariant
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblModelAction::mh_GetCheckStateVariantAsBool(const QVariant & orc_Value)
{
   const Qt::CheckState e_CheckedState = static_cast<Qt::CheckState>(orc_Value.toInt());
   bool q_Retval;

   if (e_CheckedState == Qt::Checked)
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last of those selected indices

   \param[in] orc_SelectedIndices Selected row indices

   \return
   Last selected row index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_TblModelAction::m_GetLastSelectedIndex(const std::vector<uint32> & orc_SelectedIndices) const
{
   uint32 u32_Retval;

   if (orc_SelectedIndices.size() == 0UL)
   {
      //Default value: "out of range"
      u32_Retval = this->m_GetSizeItems();
   }
   else
   {
      //Use minimal possible value as init value
      u32_Retval = 0UL;
      for (uint32 u32_It = 0UL; u32_It < orc_SelectedIndices.size(); ++u32_It)
      {
         //replace by bigger values
         u32_Retval = std::max(u32_Retval, orc_SelectedIndices[u32_It]);
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move items to target index

   \param[in] orc_ContiguousIndices Contiguous section of ascending indices
   \param[in] ou32_TargetIndex      Target index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblModelAction::m_MoveItems(const std::vector<uint32> & orc_ContiguousIndices, const uint32 ou32_TargetIndex)
{
   if (orc_ContiguousIndices.size() > 0UL)
   {
      bool q_Forward;
      if (orc_ContiguousIndices[0UL] < ou32_TargetIndex)
      {
         q_Forward = true;
      }
      else
      {
         q_Forward = false;
      }
      for (uint32 u32_ItItem = 0UL; u32_ItItem < orc_ContiguousIndices.size(); ++u32_ItItem)
      {
         if (q_Forward == true)
         {
            //There will always be a "new item" at the same position (if the move is implemented correctly!)
            this->m_MoveItem(orc_ContiguousIndices[0UL], ou32_TargetIndex);
         }
         else
         {
            //The item position will not change but the target will (if the move is implemented correctly!)
            this->m_MoveItem(orc_ContiguousIndices[u32_ItItem], ou32_TargetIndex + u32_ItItem);
         }
      }
   }
}
