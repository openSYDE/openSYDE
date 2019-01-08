//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Delegate for project file table (implementation)

   Delegate for project file table made for hovering a whole row.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.08.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>
#include "stwtypes.h"
#include "constants.h"
#include "C_PopFileTableDelegate.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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

   \created     01.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
C_PopFileTableDelegate::C_PopFileTableDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent),
   mpc_Model(NULL),
   ms32_HoveredRow(-1),
   ms32_ClickedRow(-1)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint item

   Here: special handling for boolean & hovered cells

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index

   \created     01.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_PopFileTableDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                   const QModelIndex & orc_Index) const
{
   //Draw hover effect manually
   if (orc_Index.isValid() == true)
   {
      // paint hover color
      if (orc_Index.row() == this->ms32_HoveredRow)
      {
         opc_Painter->fillRect(orc_Option.rect, QColor(96, 96, 120));
      }

      // paint pressed color
      if (orc_Index.row() == this->ms32_ClickedRow)
      {
         opc_Painter->fillRect(orc_Option.rect, QColor(138, 138, 158));
      }
   }
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set hovered row index

   \param[in] ors32_Value New hovered row index

   \return
   true  Change
   false No change

   \created     01.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
bool C_PopFileTableDelegate::SetHoveredRow(const stw_types::sint32 & ors32_Value)
{
   bool q_Retval;

   if (this->ms32_HoveredRow == ors32_Value)
   {
      q_Retval = false;
   }
   else
   {
      q_Retval = true;
   }
   this->ms32_HoveredRow = ors32_Value;
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set clicked row index

   \param[in] ors32_Value  New clicked row index

   \return
   true  Change
   false No change

   \created     01.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
bool C_PopFileTableDelegate::SetClickedRow(const stw_types::sint32 & ors32_Value)
{
   bool q_Retval;

   if (this->ms32_ClickedRow == ors32_Value)
   {
      q_Retval = false;
   }
   else
   {
      q_Retval = true;
   }
   this->ms32_ClickedRow = ors32_Value;
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set model for column look up

   \param[in] opc_Model Model for column look up

   \created     01.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_PopFileTableDelegate::SetModel(const QAbstractTableModel * const opc_Value)
{
   this->mpc_Model = opc_Value;
}
