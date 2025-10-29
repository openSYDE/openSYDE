//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic filter for a multi column model displaying leaf items as result

   Generic filter for a multi column model displaying leaf items as result

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include <QRegularExpression>

#include "stwtypes.hpp"
#include "C_TblTreMultiColumnLeafSortFilter.hpp"

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
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreMultiColumnLeafSortFilter::C_TblTreMultiColumnLeafSortFilter() :
   C_TblTreDataElementSortFilter()

{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if index contains keyword

   \param[in]  orc_Index   Index

   \return
   True  Accepted
   False Not accepted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreMultiColumnLeafSortFilter::m_Contains(const QModelIndex & orc_Index) const
{
   bool q_Retval = false;

   for (int32_t s32_Col = 0; (s32_Col < orc_Index.model()->columnCount()) && (q_Retval == false); ++s32_Col)
   {
      const QModelIndex c_Index = orc_Index.model()->index(orc_Index.row(), s32_Col, orc_Index.parent());
      const QString c_Name = orc_Index.model()->data(c_Index, static_cast<int32_t>(Qt::DisplayRole)).toString();
      q_Retval = c_Name.contains(filterRegularExpression());
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Less than comparison

   \param[in]  orc_SourceLeft    Source left model index
   \param[in]  orc_SourceRight   Source right model index

   \return
   Flags

   \retval   True    orc_SourceLeft less than orc_SourceRight
   \retval   False   Otherwise
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreMultiColumnLeafSortFilter::lessThan(const QModelIndex & orc_SourceLeft,
                                                 const QModelIndex & orc_SourceRight) const
{
   bool q_Retval;

   if (((orc_SourceLeft.isValid()) && (orc_SourceRight.isValid())) &&
       ((orc_SourceLeft.data().toString() == "") && (orc_SourceRight.data().toString() == "")))
   {
      const QModelIndex c_ParentChildLeft =
         orc_SourceLeft.model()->index(orc_SourceLeft.row(), 0, orc_SourceLeft.parent());
      const QModelIndex c_ParentChildRight = orc_SourceRight.model()->index(
         orc_SourceRight.row(), 0, orc_SourceRight.parent());
      q_Retval = true;
      for (int32_t s32_ChildLeft = 0;
           (s32_ChildLeft < orc_SourceLeft.model()->rowCount(c_ParentChildLeft)) && (q_Retval == true);
           ++s32_ChildLeft)
      {
         const QModelIndex c_ChildLeft = orc_SourceLeft.model()->index(s32_ChildLeft,
                                                                       orc_SourceLeft.column(), c_ParentChildLeft);
         for (int32_t s32_ChildRight = 0;
              (s32_ChildRight < orc_SourceRight.model()->rowCount(c_ParentChildRight)) && (q_Retval == true);
              ++s32_ChildRight)
         {
            const QModelIndex c_ChildRight = orc_SourceRight.model()->index(s32_ChildRight,
                                                                            orc_SourceRight.column(),
                                                                            c_ParentChildRight);
            if (!this->lessThan(c_ChildLeft, c_ChildRight))
            {
               q_Retval = false;
            }
         }
      }
   }
   else
   {
      q_Retval = C_TblTreDataElementSortFilter::lessThan(orc_SourceLeft, orc_SourceRight);
   }
   return q_Retval;
}
