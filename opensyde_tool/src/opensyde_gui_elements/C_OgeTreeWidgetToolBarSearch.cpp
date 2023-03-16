//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget for toolbar search (implementation)

   Tree widget for toolbar search
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include <QScrollBar>
#include "stwtypes.hpp"

#include "C_OgeTreeWidgetToolBarSearch.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

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
C_OgeTreeWidgetToolBarSearch::C_OgeTreeWidgetToolBarSearch(QWidget * const opc_Parent) :
   QTreeWidget(opc_Parent),
   mc_SearchString(""),
   mq_ResultsFound(false)
{
   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTreeWidgetToolBarSearch::~C_OgeTreeWidgetToolBarSearch()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns a flag if search results exist

   \return
   true     Results found
   false    No results
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeTreeWidgetToolBarSearch::HasResults(void) const
{
   return this->mq_ResultsFound;
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeTreeWidgetToolBarSearch::m_MarkResultString(QString & orc_ResultString) const
{
   const int32_t s32_Pos = orc_ResultString.indexOf(this->mc_SearchString, 0, Qt::CaseInsensitive);

   // mark the search string in the result
   orc_ResultString.insert(s32_Pos + this->mc_SearchString.length(), "</b>");
   orc_ResultString.insert(s32_Pos, "<b>");
}
