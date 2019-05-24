//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate for topology list (implementation)

   Delegate for topology list

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_NagTopListDelegate.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_NagTopListDelegate::C_NagTopListDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overloaded size hint

   \param[in] orc_Option Option
   \param[in] orc_Index  Index

   \return
   Current required size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_NagTopListDelegate::sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const
{
   QSize c_Retval = QStyledItemDelegate::sizeHint(orc_Option, orc_Index);

   if (orc_Index.isValid() == true)
   {
      c_Retval.setHeight(25);
   }

   return c_Retval;
}
