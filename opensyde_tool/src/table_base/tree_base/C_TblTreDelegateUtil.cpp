//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for any delegate

   Utility functions for any delegate

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QSvgRenderer>

#include "stwtypes.h"
#include "constants.h"
#include "C_TblTreDelegateUtil.h"

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
/*! \brief  Paint icon cell

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index

   \retval   true    Painted icon
   \retval   false   Nothing done
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblTreDelegateUtil::h_PaintIcon(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                       const QModelIndex & orc_Index)
{
   bool q_Retval = false;
   const QStringList c_IconPaths = orc_Index.data(msn_USER_ROLE_ICON).toStringList();

   //Check for the number of expected strings
   if ((c_IconPaths.count() == 3) || (c_IconPaths.count() == 2))
   {
      //Step 1: convert icon size to int
      bool q_Ok;
      const QString c_SizeString = c_IconPaths.at(0);
      const sintn sn_Size = c_SizeString.toInt(&q_Ok);
      if (q_Ok)
      {
         const QSize c_IconSize(sn_Size, sn_Size);
         // center icon
         const QRect c_Rect(orc_Option.rect.topLeft() + QPoint((orc_Option.rect.width() - c_IconSize.width()) / 2,
                                                               (orc_Option.rect.height() - c_IconSize.height()) / 2),
                            c_IconSize);
         q_Retval = true;
         if (c_IconPaths.count() == 2)
         {
            //Always same icon
            QSvgRenderer c_Renderer(c_IconPaths.at(1));
            c_Renderer.render(opc_Painter, c_Rect);
         }
         else
         {
            //Use specific icon
            if (orc_Index.flags().testFlag(Qt::ItemIsEditable) == true)
            {
               QSvgRenderer c_Renderer(c_IconPaths.at(1));
               c_Renderer.render(opc_Painter, c_Rect);
            }
            else
            {
               QSvgRenderer c_Renderer(c_IconPaths.at(2));
               c_Renderer.render(opc_Painter, c_Rect);
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblTreDelegateUtil::C_TblTreDelegateUtil(void)
{
}
