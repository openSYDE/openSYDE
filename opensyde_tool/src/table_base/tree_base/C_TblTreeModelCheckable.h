//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base tree model implementation with checkable items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTREEMODELCHECKABLE_H
#define C_TBLTREEMODELCHECKABLE_H
C_TBLTREEMODELCHECKABLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_TblTreModel.h"
#include "C_TblTreItem.h"

#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreeModelCheckableItem :
   public C_TblTreItem
{
public:
   C_TblTreeModelCheckableItem(void);

   Qt::CheckState e_CheckState;
   bool q_CheckBoxVisible;
};

class C_TblTreeModelCheckable :
   public C_TblTreModel
{
   Q_OBJECT

public:
   explicit C_TblTreeModelCheckable(QObject * const opc_Parent = NULL);
   ~C_TblTreeModelCheckable();

   virtual stw_types::sint32 Init(const std::vector<stw_types::uint32> & orc_ElementIndices) = 0;
   virtual void GetCheckedItems(std::vector<stw_types::uint32> & orc_ElementIndices,
                                std::vector<std::vector<stw_types::uint32> > & orc_ChildIndicesPerElement) const = 0;
   virtual stw_types::uint32 GetCheckedItemCount(void) const = 0;

   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role) override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

protected:
   void m_CheckChildren(C_TblTreeModelCheckableItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                        QModelIndex & orc_EndIndex);
   void m_CheckParent(const C_TblTreeModelCheckableItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                      QModelIndex & orc_StartIndex);
   void m_CleanUpLastModel(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
