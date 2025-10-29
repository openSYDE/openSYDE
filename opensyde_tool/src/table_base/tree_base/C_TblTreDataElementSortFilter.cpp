//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filter for data elements tree model (implementation)

   Filter for data elements tree model

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QRegularExpression>

#include "C_TblTreDataElementSortFilter.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreDataElementSortFilter::C_TblTreDataElementSortFilter(QObject * const opc_Parent) :
   QSortFilterProxyModel(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set filter string

   \param[in]  orc_Text    Text to filter for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblTreDataElementSortFilter::SetFilter(const QString & orc_Text)
{
   this->beginResetModel();
   this->setFilterFixedString(orc_Text);
   this->endResetModel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if current row is accepted for filter

   \param[in]  os32_SourceRow    Source row
   \param[in]  orc_SourceParent  Source parent

   \return
   True  Accepted
   False Not accepted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreDataElementSortFilter::filterAcceptsRow(const int32_t os32_SourceRow,
                                                     const QModelIndex & orc_SourceParent) const
{
   bool q_Retval = false;

   QList<QModelIndex> c_Children;

   //Current index
   c_Children.append(sourceModel()->index(os32_SourceRow, 0, orc_SourceParent));

   //Parse current index & children
   for (int32_t s32_ItIndex = 0; (s32_ItIndex < c_Children.length()) && (q_Retval == false); ++s32_ItIndex)
   {
      const QModelIndex & rc_CurrentIndex = c_Children[s32_ItIndex];
      const int32_t s32_RowCount = sourceModel()->rowCount(rc_CurrentIndex);
      // Add ALL children to be checked
      for (int32_t s32_ItSubChild = 0; (s32_ItSubChild < s32_RowCount) && (q_Retval == false); ++s32_ItSubChild)
      {
         q_Retval = filterAcceptsRow(s32_ItSubChild, rc_CurrentIndex);
      }

      //Check current string
      if (q_Retval == false)
      {
         q_Retval = this->m_Contains(rc_CurrentIndex);
      }
   }

   //Check parents -> if parent is valid child should appear
   if (q_Retval == false)
   {
      q_Retval = this->m_CheckParents(orc_SourceParent);
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if index contains keyword

   \param[in]  orc_Index   Index

   \return
   True  Accepted
   False Not accepted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreDataElementSortFilter::m_Contains(const QModelIndex & orc_Index) const
{
   const QString c_Name = sourceModel()->data(orc_Index, static_cast<int32_t>(Qt::DisplayRole)).toString();
   const bool q_Retval = c_Name.contains(filterRegularExpression());

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check parents for keyword

   \param[in]  orc_SourceParent  Source parent

   \return
   True  Accepted
   False Not accepted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreDataElementSortFilter::m_CheckParents(const QModelIndex & orc_SourceParent) const
{
   bool q_Retval = false;

   if (orc_SourceParent.isValid())
   {
      q_Retval = m_Contains(orc_SourceParent);
      if (q_Retval == false)
      {
         q_Retval = m_CheckParents(orc_SourceParent.parent());
      }
   }
   return q_Retval;
}
