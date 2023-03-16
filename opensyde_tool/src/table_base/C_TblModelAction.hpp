//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table model base class providing an interface for common actions (as far as possible) (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLMODELACTION_H
#define C_TBLMODELACTION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAbstractTableModel>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblModelAction :
   public QAbstractTableModel
{
   Q_OBJECT

public:
   C_TblModelAction(QObject * const opc_Parent = NULL);

   int32_t rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;

   uint32_t AddNewItem(const std::vector<uint32_t> & orc_SelectedIndices);

   //-----------------------------------------------------------------------------
   /*!
      \brief   Handle copy items action

      This is not yet implemented and needs to be implemented by inheritors,
      because a generic function would not make anything easier as any copy action depends on the data.

      \param[in] orc_SelectedIndices Selected row indices (Expected: unique)
   */
   //-----------------------------------------------------------------------------
   virtual void CopySelectedItems(const std::vector<uint32_t> & orc_SelectedIndices) const = 0;
   std::vector<uint32_t> PasteItems(const std::vector<uint32_t> & orc_SelectedIndices);
   uint32_t CutSelectedItems(const std::vector<uint32_t> & orc_SelectedIndices);
   uint32_t DeleteSelectedItems(const std::vector<uint32_t> & orc_SelectedIndices);
   void MoveSelectedItems(const std::vector<uint32_t> & orc_SelectedIndices, const bool oq_Up);
   bool CheckAllIndicesInRange(const std::vector<uint32_t> & orc_Indices) const;

   //Utility
   void ReloadAll(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigItemCountChanged(const uint32_t ou32_NewItemCount);

protected:
   //-----------------------------------------------------------------------------
   /*!
      \brief   Handle add new item action

      \param[in] ou32_SelectedIndex Index to insert item at

      \return
      Index of new item
   */
   //-----------------------------------------------------------------------------
   virtual uint32_t m_AddNewItem(const uint32_t ou32_SelectedIndex) = 0;
   //-----------------------------------------------------------------------------
   /*!
      \brief   Handle paste items action

      \param[in] ou32_SelectedIndex Index to insert item at

      \return
      Indices of new items
   */
   //-----------------------------------------------------------------------------
   virtual std::vector<uint32_t> m_PasteItems(const uint32_t ou32_SelectedIndex) = 0;
   //-----------------------------------------------------------------------------
   /*!
      \brief   Get size of item container, i.e. number of items.

      \return
      Size of item container
   */
   //-----------------------------------------------------------------------------
   virtual uint32_t m_GetSizeItems(void) const = 0;
   //-----------------------------------------------------------------------------
   /*!
      \brief   Delete specific item

      Warning: not expected to fail

      \param[in] ou32_Index Index to delete
   */
   //-----------------------------------------------------------------------------
   virtual void m_DeleteItem(const uint32_t ou32_Index) = 0;
   //-----------------------------------------------------------------------------
   /*!
      \brief   Move specific item

      For implementation: First delete ou32_SourceIndex then insert ou32_TargetIndex
      Warning: not expected to fail

      \param[in] ou32_SourceIndex Source index
      \param[in] ou32_TargetIndex Target index
   */
   //-----------------------------------------------------------------------------
   virtual void m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex) = 0;

   virtual void m_BeginRemoveRows(const uint32_t ou32_FirstIndex, const uint32_t ou32_LastIndex);
   virtual void m_EndRemoveRows(const uint32_t ou32_FirstIndex, const uint32_t ou32_LastIndex);

   static QVariant mh_HandleHexValue(const uint64_t ou64_Value, const int32_t os32_Role);
   static QVariant mh_GetBoolAsCheckStateVariant(const bool oq_Value);
   static bool mh_GetCheckStateVariantAsBool(const QVariant & orc_Value);
   uint32_t m_GetLastSelectedIndex(const std::vector<uint32_t> & orc_SelectedIndices) const;

private:
   void m_MoveItems(const std::vector<uint32_t> & orc_ContiguousIndices, const uint32_t ou32_TargetIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
