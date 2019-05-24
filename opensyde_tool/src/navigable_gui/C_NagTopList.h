//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for topology widgets (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOPLIST_H
#define C_NAGTOPLIST_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_NagTopListModel.h"
#include "C_NagTopListDelegate.h"
#include "C_OgeListViewToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagTopList :
   public stw_opensyde_gui_elements::C_OgeListViewToolTipBase
{
   Q_OBJECT

public:
   C_NagTopList(QWidget * const opc_Parent = NULL);

   void SetTypeNode(const bool oq_IsNode);
   void SetSelectedIndexAndScroll(const stw_types::sint32 os32_Selected);
   void SetContent(const std::vector<QString> & orc_Content);
   void SetError(const std::vector<bool> & orc_Error);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigClicked(const stw_types::sintn osn_Index);

private:
   C_NagTopListModel mc_Model;
   C_NagTopListDelegate mc_Delegate;

   void m_HandleClicked(const QModelIndex & orc_Index);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
