//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic filter for a multi column model displaying leaf items as result

   Generic filter for a multi column model displaying leaf items as result

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_TblTreMultiColumnLeafSortFilter.h"

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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreMultiColumnLeafSortFilter::C_TblTreMultiColumnLeafSortFilter()
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

   for (sintn sn_Col = 0; (sn_Col < orc_Index.model()->columnCount()) && (q_Retval == false); ++sn_Col)
   {
      const QModelIndex c_Index = orc_Index.model()->index(orc_Index.row(), sn_Col, orc_Index.parent());
      const QString c_Name = orc_Index.model()->data(c_Index, static_cast<sintn>(Qt::DisplayRole)).toString();
      q_Retval = c_Name.contains(filterRegExp());
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
      for (sintn sn_ChildLeft = 0;
           (sn_ChildLeft < orc_SourceLeft.model()->rowCount(c_ParentChildLeft)) && (q_Retval == true);
           ++sn_ChildLeft)
      {
         const QModelIndex c_ChildLeft = orc_SourceLeft.model()->index(sn_ChildLeft,
                                                                       orc_SourceLeft.column(), c_ParentChildLeft);
         for (sintn sn_ChildRight = 0;
              (sn_ChildRight < orc_SourceRight.model()->rowCount(c_ParentChildRight)) && (q_Retval == true);
              ++sn_ChildRight)
         {
            const QModelIndex c_ChildRight = orc_SourceRight.model()->index(sn_ChildRight,
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
