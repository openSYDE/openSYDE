//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Tree widget for toolbar search (implementation)

   Tree widget for toolbar search
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.03.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"
#include <QScrollBar>
#include "stwtypes.h"

#include "C_OgeTreeWidgetToolBarSearch.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;

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

   \created     31.01.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_OgeTreeWidgetToolBarSearch::C_OgeTreeWidgetToolBarSearch(QWidget * const opc_Parent) :
   QTreeWidget(opc_Parent),
   mc_SearchString(""),
   mq_ResultsFound(false)
{
   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     16.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeTreeWidgetToolBarSearch::~C_OgeTreeWidgetToolBarSearch()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns a flag if search results exist

   \return
   true     Results found
   false    No results

   \created     13.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OgeTreeWidgetToolBarSearch::HasResults(void) const
{
   return this->mq_ResultsFound;
}

//-----------------------------------------------------------------------------
void C_OgeTreeWidgetToolBarSearch::m_DeleteChildren(QTreeWidgetItem * const opc_ItemParent) const
{
   while (opc_ItemParent->childCount() > 0)
   {
      QTreeWidgetItem * pc_Item = opc_ItemParent->child(0);
      if (pc_Item != NULL)
      {
         opc_ItemParent->removeChild(pc_Item);
         delete pc_Item;
      }
   }
}

//-----------------------------------------------------------------------------
void C_OgeTreeWidgetToolBarSearch::m_MarkResultString(QString & orc_ResultString) const
{
   const sintn sn_Pos = orc_ResultString.indexOf(this->mc_SearchString, 0, Qt::CaseInsensitive);

   // mark the search string in the result
   orc_ResultString.insert(sn_Pos + this->mc_SearchString.length(), "</b>");
   orc_ResultString.insert(sn_Pos, "<b>");
}
