//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for code generation items

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDCODEGENERATIONMODEL_H
#define C_SDCODEGENERATIONMODEL_H

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

class C_SdCodeGenerationItem :
   public C_TblTreItem
{
public:
   C_SdCodeGenerationItem(void);
   Qt::CheckState e_CheckState;

   bool q_Checkable;
};

class C_SdCodeGenerationModel :
   public C_TblTreModel
{
   Q_OBJECT

public:
   explicit C_SdCodeGenerationModel(QObject * const opc_Parent = NULL);
   ~C_SdCodeGenerationModel();

   stw_types::uint32 Init(const std::vector<stw_types::uint32> & orc_NodeIndices);
   void GetCheckedItems(std::vector<stw_types::uint32> & orc_NodeIndices,
                        std::vector<std::vector<stw_types::uint32> > & orc_AppIndicesPerNode) const;
   stw_types::uint32 GetCheckedItemCount(void) const;

   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role) override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

private:
   void m_CheckChildren(C_SdCodeGenerationItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                        QModelIndex & orc_EndIndex);
   void m_CheckParent(const C_SdCodeGenerationItem * const opc_TreeItem, const QModelIndex & orc_ItemIndex,
                      QModelIndex & orc_StartIndex);
   void m_CleanUpLastModel(void);
   stw_types::uint32 m_CheckInitItems(C_SdCodeGenerationItem & orc_VisibleRootItem,
                                      const std::vector<stw_types::uint32> & orc_NodeIndices) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
