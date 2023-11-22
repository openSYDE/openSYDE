//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for topology item navigation tree
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOPTREEVIEW_HPP
#define C_NAGTOPTREEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_NagTopTreeModel.hpp"
#include "C_NagTopTreeDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagTopTreeView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_NagTopTreeView(QWidget * const opc_Parent = NULL);

   void SetTypeNode(const bool oq_IsNode);
   void SetSelectedIndexAndScroll(const int32_t os32_Selected);
   void SetContent(const std::vector<QString> & orc_Content,
                   const std::vector<std::vector<QString> > & orc_SubContent = std::vector<std::vector<QString> >());
   void SetError(const std::vector<bool> & orc_Error);
   void UpdateItem(const uint32_t ou32_Index, const QString & orc_Content);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigClicked(const int32_t os32_Index);

protected:
   void drawBranches(QPainter * const opc_Painter, const QRect & orc_Rect,
                     const QModelIndex & orc_Index) const override;

private:
   stw::opensyde_gui_logic::C_NagTopTreeModel mc_Model;
   stw::opensyde_gui::C_NagTopTreeDelegate mc_Delegate;

   void m_OnItemSelected(const QModelIndex & orc_Index);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
