//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model for topology item navigation tree
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOPTREEMODEL_H
#define C_NAGTOPTREEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblTreModel.hpp"
#include "C_TblTreItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagTopTreeModel :
   public C_TblTreModel
{
public:
   C_NagTopTreeModel(QObject * const opc_Parent = NULL);
   ~C_NagTopTreeModel(void) override;

   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;

   void SetTypeNode(const bool oq_IsNode);
   void SetContent(const std::vector<QString> & orc_Content,
                   const std::vector<std::vector<QString> > & orc_SubContent = std::vector<std::vector<QString> >());
   void SetError(const std::vector<bool> & orc_Error);
   void UpdateItem(const uint32_t ou32_Index, const QString & orc_Content);
   void Clear(void);
   static uint32_t h_GetFlatIndexFromModelIndex(const QModelIndex & orc_ModelIndex);
   QModelIndex GetModelIndexFromFlatIndex(const uint32_t ou32_FlatIndex);

private:
   bool mq_TypeNode;
   const QIcon mc_ErrorIcon;

   void m_SetChildError(C_TblTreItem * const opc_Item, const bool oq_Error);
   void m_SetParentError(C_TblTreItem * const opc_Item, const bool oq_Error, const QString & orc_ChildNames);
   void m_SetItemError(C_TblTreItem * const opc_Item, const bool oq_Error, const QString & orc_ToolTipContent);
   QIcon m_GetPlaceholderIcon(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
