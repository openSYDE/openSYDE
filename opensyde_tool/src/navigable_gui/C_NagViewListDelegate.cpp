//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for view list (implementation)

   Delegate for view list

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_NagViewList.h"
#include "C_NagViewListDelegate.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagViewListDelegate::C_NagViewListDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mpc_View(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register current view

   \param[in] opc_View Current view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewListDelegate::SetListView(QListView * const opc_View)
{
   this->mpc_View = opc_View;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register expand change

   \param[in] orc_Index Changed index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagViewListDelegate::RegisterExpand(const QModelIndex & orc_Index)
{
   Q_EMIT this->sizeHintChanged(orc_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overloaded size hint

   \param[in] orc_Option Option
   \param[in] orc_Index  Index

   \return
   Current required size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_NagViewListDelegate::sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const
{
   sintn sn_Height = 1;
   QSize c_Retval = QStyledItemDelegate::sizeHint(orc_Option, orc_Index);

   if (orc_Index.isValid() == true)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      const C_NagViewList * const pc_View = dynamic_cast<const C_NagViewList * const>(this->mpc_View);

      if (pc_View != NULL)
      {
         const C_NagViewItem * const pc_Item = pc_View->GetItemAt(orc_Index.row());
         if (pc_Item != NULL)
         {
            sn_Height += pc_Item->sizeHint().height();
         }
      }
   }
   c_Retval.setHeight(sn_Height);
   return c_Retval;
}
