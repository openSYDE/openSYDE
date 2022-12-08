//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base tree model implementation with checkable items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLTREEMODELCHECKABLE_HPP
#define C_TBLTREEMODELCHECKABLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_TblTreModel.hpp"
#include "C_TblTreItem.hpp"

#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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
   ~C_TblTreeModelCheckable() override;

   virtual int32_t Init(const std::vector<uint32_t> & orc_ElementIndices) = 0;
   virtual void GetCheckedItems(std::vector<uint32_t> & orc_ElementIndices,
                                std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerElement) const = 0;
   virtual uint32_t GetCheckedItemCount(void) const = 0;

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value, const int32_t os32_Role) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

protected:
   void m_CheckChildren(C_TblTreeModelCheckableItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                        QModelIndex & orc_EndIndex);
   void m_CheckParent(const C_TblTreeModelCheckableItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                      QModelIndex & orc_StartIndex);
   void m_CleanUpLastModel(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
