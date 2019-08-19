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

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblModelAction :
   public QAbstractTableModel
{
   Q_OBJECT

public:
   C_TblModelAction(QObject * const opc_Parent = NULL);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960

   // Basic functionality:
   //lint -e{1735} Suppression, because default parameters are identical
   virtual stw_types::sintn rowCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   //lint -restore

   stw_types::uint32 AddNewItem(const std::vector<stw_types::uint32> & orc_SelectedIndices);

   //-----------------------------------------------------------------------------
   /*!
      \brief   Handle copy items action

      This is not yet implemented and needs to be implemented by inheritors,
      because a generic function would not make anything easier as any copy action depends on the data.

      \param[in] orc_SelectedIndices Selected row indices (Expected: unique)
   */
   //-----------------------------------------------------------------------------
   virtual void CopySelectedItems(const std::vector<stw_types::uint32> & orc_SelectedIndices) const = 0;
   std::vector<stw_types::uint32> PasteItems(const std::vector<stw_types::uint32> & orc_SelectedIndices);
   stw_types::uint32 CutSelectedItems(const std::vector<stw_types::uint32> & orc_SelectedIndices);
   stw_types::uint32 DeleteSelectedItems(const std::vector<stw_types::uint32> & orc_SelectedIndices);
   void MoveSelectedItems(const std::vector<stw_types::uint32> & orc_SelectedIndices, const bool oq_Up);
   bool CheckAllIndicesInRange(const std::vector<stw_types::uint32> & orc_Indices) const;

   //Utility
   void ReloadAll(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigItemCountChanged(const stw_types::uint32 ou32_NewItemCount);

protected:
   //-----------------------------------------------------------------------------
   /*!
      \brief   Handle add new item action

      \param[in] ou32_SelectedIndex Index to insert item at

      \return
      Index of new item
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::uint32 m_AddNewItem(const stw_types::uint32 ou32_SelectedIndex) = 0;
   //-----------------------------------------------------------------------------
   /*!
      \brief   Handle paste items action

      \param[in] ou32_SelectedIndex Index to insert item at

      \return
      Indices of new items
   */
   //-----------------------------------------------------------------------------
   virtual std::vector<stw_types::uint32> m_PasteItems(const stw_types::uint32 ou32_SelectedIndex) = 0;
   //-----------------------------------------------------------------------------
   /*!
      \brief   Get size of item container, i.e. number of items.

      \return
      Size of item container
   */
   //-----------------------------------------------------------------------------
   virtual stw_types::uint32 m_GetSizeItems(void) const = 0;
   //-----------------------------------------------------------------------------
   /*!
      \brief   Delete specific item

      Warning: not expected to fail

      \param[in] ou32_Index Index to delete
   */
   //-----------------------------------------------------------------------------
   virtual void m_DeleteItem(const stw_types::uint32 ou32_Index) = 0;
   //-----------------------------------------------------------------------------
   /*!
      \brief   Move specific item

      For implementation: First delete ou32_SourceIndex then insert ou32_TargetIndex
      Warning: not expected to fail

      \param[in] ou32_SourceIndex Source index
      \param[in] ou32_TargetIndex Target index
   */
   //-----------------------------------------------------------------------------
   virtual void m_MoveItem(const stw_types::uint32 ou32_SourceIndex, const stw_types::uint32 ou32_TargetIndex) = 0;

   virtual void m_BeginRemoveRows(const stw_types::uint32 ou32_FirstIndex, const stw_types::uint32 ou32_LastIndex);
   virtual void m_EndRemoveRows(const stw_types::uint32 ou32_FirstIndex, const stw_types::uint32 ou32_LastIndex);

   static QVariant mh_HandleHexValue(const stw_types::uint64 ou64_Value, const stw_types::sintn osn_Role);
   static QString mh_GetValueAsHex(const stw_types::uint64 ou64_Value);
   static QVariant mh_GetBoolAsCheckStateVariant(const bool oq_Value);
   static bool mh_GetCheckStateVariantAsBool(const QVariant & orc_Value);
   stw_types::uint32 m_GetLastSelectedIndex(const std::vector<stw_types::uint32> & orc_SelectedIndices) const;

private:
   void m_MoveItems(const std::vector<stw_types::uint32> & orc_ContiguousIndices,
                    const stw_types::uint32 ou32_TargetIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
