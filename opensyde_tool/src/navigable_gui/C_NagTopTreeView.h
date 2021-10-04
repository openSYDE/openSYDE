//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for topology item navigation tree
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOPTREEVIEW_H
#define C_NAGTOPTREEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.h"
#include "C_NagTopTreeModel.h"
#include "C_NagTopTreeDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagTopTreeView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_NagTopTreeView(QWidget * const opc_Parent = NULL);

   void SetTypeNode(const bool oq_IsNode);
   void SetSelectedIndexAndScroll(const stw_types::sint32 os32_Selected);
   void SetContent(const std::vector<QString> & orc_Content,
                   const std::vector<std::vector<QString> > & orc_SubContent = std::vector<std::vector<QString> >());
   void SetError(const std::vector<bool> & orc_Error);
   void UpdateItem(const stw_types::uint32 ou32_Index, const QString & orc_Content);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigClicked(const stw_types::sintn osn_Index);

protected:
   virtual void drawBranches(QPainter * const opc_Painter, const QRect & orc_Rect,
                             const QModelIndex & orc_Index) const override;

private:
   stw_opensyde_gui_logic::C_NagTopTreeModel mc_Model;
   stw_opensyde_gui::C_NagTopTreeDelegate mc_Delegate;

   void m_OnItemSelected(const QModelIndex & orc_Index);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
