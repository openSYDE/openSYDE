//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Delegate for topology list (implementation)

   Delegate for topology list

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     31.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_NagTopListDelegate.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     31.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_NagTopListDelegate::C_NagTopListDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overloaded size hint

   \param[in] orc_Option Option
   \param[in] orc_Index  Index

   \return
   Current required size

   \created     31.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSize C_NagTopListDelegate::sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const
{
   QSize c_Retval = QStyledItemDelegate::sizeHint(orc_Option, orc_Index);

   if (orc_Index.isValid() == true)
   {
      c_Retval.setHeight(25);
   }

   return c_Retval;
}
