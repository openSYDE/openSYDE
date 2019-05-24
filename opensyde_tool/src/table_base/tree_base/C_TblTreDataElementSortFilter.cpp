//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filter for data elements tree model (implementation)

   Filter for data elements tree model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_TblTreDataElementSortFilter.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

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
C_TblTreDataElementSortFilter::C_TblTreDataElementSortFilter(QObject * const opc_Parent) :
   QSortFilterProxyModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set filter string

   \param[in] orc_Text Text to filter for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementSortFilter::SetFilter(const QString & orc_Text)
{
   this->beginResetModel();
   this->setFilterFixedString(orc_Text);
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current row is accepted for filter

   \param[in] osn_SourceRow    Source row
   \param[in] orc_SourceParent Source parent

   \return
   True  Accepted
   False Not accepted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreDataElementSortFilter::filterAcceptsRow(const sintn osn_SourceRow,
                                                     const QModelIndex & orc_SourceParent) const
{
   bool q_Retval = false;
   QString c_Name;

   QList<QModelIndex> c_Children;

   //Current index
   c_Children.append(sourceModel()->index(osn_SourceRow, 0, orc_SourceParent));

   //Parse current index & children
   for (sintn sn_ItIndex = 0; (sn_ItIndex < c_Children.length()) && (q_Retval == false); ++sn_ItIndex)
   {
      const QModelIndex & rc_CurrentIndex = c_Children[sn_ItIndex];
      const sintn sn_RowCount = sourceModel()->rowCount(rc_CurrentIndex);
      // Add ALL children to be checked
      for (sintn sn_ItSubChild = 0; (sn_ItSubChild < sn_RowCount) && (q_Retval == false); ++sn_ItSubChild)
      {
         q_Retval = filterAcceptsRow(sn_ItSubChild, rc_CurrentIndex);
      }

      //Check current string if leaf element (no other elements are checked)
      if (sn_RowCount == 0)
      {
         c_Name = sourceModel()->data(rc_CurrentIndex, static_cast<sintn>(Qt::DisplayRole)).toString();
         q_Retval = c_Name.contains(filterRegExp());
      }
   }
   return q_Retval;
}
