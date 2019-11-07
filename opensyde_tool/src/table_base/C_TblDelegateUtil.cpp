//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for table/tree delegates (implementation)

   Utility class for table/tree delegates

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QBitArray>

#include "C_Uti.h"
#include "cam_constants.h"
#include "C_TblDelegateUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Paint marked section

   \param[in,out] opc_Painter                  Painter
   \param[in]     orc_CellRect                 Cell rectangle to draw in
   \param[in]     orc_Index                    Index
   \param[in]     oq_Selected                  Flag if item is selected
   \param[in]     orc_DefaultBackgroundColor   Default background color
   \param[in]     orc_DefaultBorderColor       Default border color
   \param[in]     orc_DefaultFont              Default font
   \param[in]     orc_HighlightForegroundColor Highlight foreground color
   \param[in]     orc_HighlightBackgroundColor Highlight background color
   \param[in]     orc_HighlightBorderColor     Highlight border color
   \param[in]     orc_HighlightFont            Highlight font
   \param[in]     osn_HexSpacing               In between spacing when using HEX format
   \param[in]     osn_DecimalSpacing           In between spacing when using decimal format

   \return
   True  Section painted
   False Section empty
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblDelegateUtil::h_PaintMarkedCell(QPainter * const opc_Painter, const QRect & orc_CellRect,
                                          const QModelIndex & orc_Index, const bool oq_Selected,
                                          const QColor & orc_DefaultBackgroundColor,
                                          const QColor & orc_DefaultBorderColor, const QFont & orc_DefaultFont,
                                          const QColor & orc_HighlightForegroundColor,
                                          const QColor & orc_HighlightBackgroundColor,
                                          const QColor & orc_HighlightBorderColor, const QFont & orc_HighlightFont,
                                          const sintn osn_HexSpacing, const sintn osn_DecimalSpacing)
{
   bool q_Retval = true;
   QBitArray c_Array;

   //Get data AFTER unlocking the data
   //Get the highlighted sections (Warning: only expected to be non zero for data column
   c_Array = orc_Index.data(msn_USER_ROLE_MARKER).toBitArray();

   //Only draw manually if necessary
   if (c_Array.isEmpty() == false)
   {
      bool q_IsHex;
      //Acquire data via special user role to not have a conflict with the default drawing engine
      const QString c_Data = orc_Index.data(msn_USER_ROLE_MARKER_TEXT).toString();
      //Separate data bytes
      const QStringList c_List = mhc_CreateAndStandardizeStringList(c_Data, q_IsHex);
      //GreyOutValues
      const QByteArray c_TransparencyValues = orc_Index.data(msn_USER_ROLE_MARKER_TRANSPARENCY).toByteArray();
      //Check if expected size (Number of bytes and flags to highlight should match)
      //Transparency may be bigger, but not smaller
      if ((c_List.size() == c_Array.size()) && (c_List.size() <= c_TransparencyValues.size()))
      {
         const QFontMetrics c_MetricsDefault(orc_DefaultFont);
         const QColor c_ForegroundInformation =
            orc_Index.data(static_cast<sint32>(Qt::ForegroundRole)).value<QColor>();
         QColor c_NonHighlightedColorWithTransparency = c_ForegroundInformation;
         const QColor c_HighlightedColorWithTransparency = orc_HighlightForegroundColor;
         sint32 s32_SectionWidth;
         QStringList c_P1;
         QStringList c_P2;
         QStringList c_P3;
         sint32 s32_SelectionMargin;
         QRect c_RectSegment;
         bool q_Start = true;
         sintn sn_ItTransparency = 0;
         QPoint c_CurTopLeft = orc_CellRect.topLeft();
         const sint32 s32_HalfSpaceWidth = static_cast<sint32>(c_MetricsDefault.width(" ") / 2);

         //Handle dynamic section width
         if (q_IsHex)
         {
            //Hex section width
            s32_SectionWidth = osn_HexSpacing;
            s32_SelectionMargin = -s32_HalfSpaceWidth * 2L;
         }
         else
         {
            //Decimal section width (one more number)
            s32_SectionWidth = osn_DecimalSpacing;
            s32_SelectionMargin = -s32_HalfSpaceWidth * 4L;
         }

         //Handle selected item color
         if (oq_Selected)
         {
            //if selected use selected foreground color
            c_NonHighlightedColorWithTransparency = orc_HighlightForegroundColor;
         }
         else
         {
            //For now skip in selected case as only this column is adapted
            //handle transparency (AFTER replacing highlight color if necessary)
            c_NonHighlightedColorWithTransparency.setAlpha(c_ForegroundInformation.alpha());
            //Skip selected color -> currently all selected items don't have transparency
            //c_HighlightedColorWithTransparency.setAlpha(c_ForegroundInformation.alpha());
         }

         //Extract string parts
         for (sint32 s32_ItLits = 0UL; s32_ItLits < c_List.size(); ++s32_ItLits)
         {
            //Next byte value
            const QString & rc_NewItem = c_List.at(s32_ItLits);
            //Check if highlighted
            if (c_Array.at(s32_ItLits) == true)
            {
               //While processing the highlighted section append to the second vector/string
               q_Start = false;
               c_P2.append(rc_NewItem);
            }
            else
            {
               //While not reaching the start of the highlighted section append to the first vector/string
               if (q_Start == true)
               {
                  c_P1.append(rc_NewItem);
               }
               else
               {
                  //Beyond reaching the end of the highlighted section append to the last vector/string
                  c_P3.append(rc_NewItem);
               }
            }
         }

         opc_Painter->save();
         //Draw start section (before highlighted area)
         //Each item separate: so each item can have the same section width
         for (QStringList::const_iterator c_It = c_P1.begin(); c_It != c_P1.end(); ++c_It)
         {
            const QColor c_TextColor =
               oq_Selected ? c_NonHighlightedColorWithTransparency : C_TblDelegateUtil::mh_GetColorTransparent(
                  c_NonHighlightedColorWithTransparency, c_TransparencyValues.at(sn_ItTransparency));
            C_TblDelegateUtil::h_PaintStringWithRestriction(opc_Painter, c_CurTopLeft, orc_CellRect, *c_It,
                                                            orc_DefaultBackgroundColor,
                                                            orc_DefaultBorderColor,
                                                            c_TextColor,
                                                            orc_DefaultFont,
                                                            Qt::AlignLeft | Qt::AlignVCenter, s32_SectionWidth,
                                                            s32_HalfSpaceWidth, s32_HalfSpaceWidth);
            //Transparency iteration
            ++sn_ItTransparency;
         }

         //Handle highlighted section
         if (c_P2.isEmpty() == false)
         {
            //Draw highlighted section background (separate as this should be a continuous section)
            //Use the current top left NOT the one from the start -> don't move this line to the start!
            c_RectSegment =
               QRect(c_CurTopLeft,
                     QSize(static_cast<sintn>(static_cast<sint32>(c_P2.size()) * s32_SectionWidth),
                           orc_CellRect.height()));
            opc_Painter->setBrush(orc_HighlightBackgroundColor);
            opc_Painter->setPen(orc_HighlightBorderColor);
            //Offset for last space
            opc_Painter->drawRect(c_RectSegment.adjusted(0, 0, s32_SelectionMargin, 0));

            //Draw highlighted section foreground
            //Each item separate: so each item can have the same section width
            for (QStringList::const_iterator c_It = c_P2.begin(); c_It != c_P2.end(); ++c_It)
            {
               C_TblDelegateUtil::h_PaintStringWithRestriction(opc_Painter, c_CurTopLeft, orc_CellRect, *c_It,
                                                               Qt::transparent, Qt::transparent,
                                                               c_HighlightedColorWithTransparency,
                                                               orc_HighlightFont,
                                                               Qt::AlignLeft | Qt::AlignVCenter, s32_SectionWidth,
                                                               s32_HalfSpaceWidth, s32_HalfSpaceWidth);
               //Transparency iteration
               ++sn_ItTransparency;
            }
         }

         //Draw parts after highlighted section (if any)
         //Each item separate: so each item can have the same section width
         for (QStringList::const_iterator c_It = c_P3.begin(); c_It != c_P3.end(); ++c_It)
         {
            const QColor c_TextColor =
               oq_Selected ? c_NonHighlightedColorWithTransparency : C_TblDelegateUtil::mh_GetColorTransparent(
                  c_NonHighlightedColorWithTransparency, c_TransparencyValues.at(sn_ItTransparency));
            C_TblDelegateUtil::h_PaintStringWithRestriction(opc_Painter, c_CurTopLeft, orc_CellRect, *c_It,
                                                            orc_DefaultBackgroundColor,
                                                            orc_DefaultBorderColor,
                                                            c_TextColor,
                                                            orc_DefaultFont,
                                                            Qt::AlignLeft | Qt::AlignVCenter, s32_SectionWidth,
                                                            s32_HalfSpaceWidth, s32_HalfSpaceWidth);
            //Transparency iteration
            ++sn_ItTransparency;
         }

         opc_Painter->restore();
      }
      else
      {
         q_Retval = false;
      }
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Paint string segment and update current top left position

   Warning: painter is not saved/restored

   \param[in,out] opc_Painter      Painter
   \param[in,out] orc_TopLeft      Top left of current position to draw at (updated if anything gets drawn
   \param[in]     orc_CompleteCell Complete cell to always draw in
   \param[in]     orc_Text         Text to draw
   \param[in]     orc_Background   Background color
   \param[in]     orc_Border       Border color
   \param[in]     orc_Foreground   Foreground color
   \param[in]     orc_Font         Font
   \param[in]     orc_Alignment    Text alignment flags
   \param[in]     os32_CellWidth   Cell width to use if possible
   \param[in]     os32_MarginFront Margin at front if any
   \param[in]     os32_MarginBack  Margin at back if any

   \return
   True  Text got cut off by complete cell restrictions
   False Complete text drawn
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblDelegateUtil::h_PaintStringWithRestriction(QPainter * const opc_Painter, QPoint & orc_TopLeft,
                                                     const QRect & orc_CompleteCell, const QString & orc_Text,
                                                     const QColor & orc_Background, const QColor & orc_Border,
                                                     const QColor & orc_Foreground, const QFont & orc_Font,
                                                     const QFlags<Qt::AlignmentFlag> & orc_Alignment,
                                                     const sint32 os32_CellWidth, const sint32 os32_MarginFront,
                                                     const sint32 os32_MarginBack)
{
   bool q_Retval;
   const QTextOption c_Option(orc_Alignment);
   QString c_DrawnText = orc_Text;
   const sint32 s32_SegmentWidth = C_TblDelegateUtil::mh_GetSegmentWidth(orc_TopLeft, orc_CompleteCell, c_DrawnText,
                                                                         orc_Font, os32_CellWidth, os32_MarginFront,
                                                                         os32_MarginBack, q_Retval);
   //Rect segment to draw in
   const QRect c_RectSegment(orc_TopLeft, QSize(s32_SegmentWidth, orc_CompleteCell.height()));

   //Background
   if ((orc_Border.alpha() > 0) || (orc_Background.alpha() > 0))
   {
      opc_Painter->setPen(orc_Border);
      opc_Painter->setBrush(orc_Background);
      opc_Painter->drawRect(c_RectSegment);
   }
   //Foreground
   if (orc_Foreground.alpha() > 0)
   {
      opc_Painter->setPen(orc_Foreground);
      opc_Painter->setBrush(Qt::NoBrush);
      opc_Painter->setFont(orc_Font);
      opc_Painter->drawText(c_RectSegment.adjusted(os32_MarginFront, 0, 0, 0), c_DrawnText, c_Option);
   }
   //Update point
   orc_TopLeft.setX(orc_TopLeft.x() + static_cast<sintn>(s32_SegmentWidth));
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblDelegateUtil::C_TblDelegateUtil(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get segment width for the current text, restricted to the cell width

   \param[in]     orc_TopLeft      Top left of current position to draw at (updated if anything gets drawn
   \param[in]     orc_CompleteCell Complete cell to always draw in
   \param[in,out] orc_DrawnText    Text to draw
   \param[in]     orc_Font         Font
   \param[in]     os32_CellWidth   Cell width to use if possible
   \param[in]     os32_MarginFront Margin at front if any
   \param[in]     os32_MarginBack  Margin at back if any
   \param[in,out] orq_Changed      Flag if cell end was reached

   \return
   Calculated segment width
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_TblDelegateUtil::mh_GetSegmentWidth(const QPoint & orc_TopLeft, const QRect & orc_CompleteCell,
                                             QString & orc_DrawnText, const QFont & orc_Font,
                                             const sint32 os32_CellWidth, const sint32 os32_MarginFront,
                                             const sint32 os32_MarginBack, bool & orq_Changed)
{
   sint32 s32_SegmentWidth = os32_CellWidth;
   const QFontMetrics c_Metrics(orc_Font);

   //Default
   orq_Changed = false;

   //If user does not specify anything use the metrics width instead
   if (s32_SegmentWidth < 0)
   {
      s32_SegmentWidth = os32_MarginFront + c_Metrics.width(orc_DrawnText) + os32_MarginBack;
   }
   //Check if the segment width does exceed the complete cell boundaries
   if ((static_cast<sint32>(orc_TopLeft.x()) + s32_SegmentWidth) >
       static_cast<sint32>(orc_CompleteCell.topLeft().x() + orc_CompleteCell.width()))
   {
      s32_SegmentWidth =
         static_cast<sint32>((orc_CompleteCell.topLeft().x() + orc_CompleteCell.width()) - orc_TopLeft.x()) -
         os32_MarginFront;
      orc_DrawnText = C_Uti::h_AdaptStringToSize(orc_DrawnText, c_Metrics, static_cast<sintn>(s32_SegmentWidth));
      orq_Changed = true;
   }
   return s32_SegmentWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create string list from input and standardize the segment lengths as necessary

   \param[in]     orc_Input Input to segment and standardize
   \param[in,out] orq_IsHex Utility parameter to identify if the values are hex or decimal

   \return
   Segmented and standardized string parts
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_TblDelegateUtil::mhc_CreateAndStandardizeStringList(const QString & orc_Input, bool & orq_IsHex)
{
   QStringList c_Retval = orc_Input.split(" ", QString::SkipEmptyParts);

   if (orc_Input.size() > 3)
   {
      if (orc_Input.at(2) == ' ')
      {
         orq_IsHex = true;
      }
      else
      {
         orq_IsHex = false;
      }
   }
   else
   {
      orq_IsHex = false;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get color with specified transparency value

   \param[in] orc_ColorBase    Base color
   \param[in] ocn_Transparency Desired transparency value

   \return
   Color with specified transparency value
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_TblDelegateUtil::mh_GetColorTransparent(const QColor & orc_ColorBase, const charn ocn_Transparency)
{
   QColor c_Retval = orc_ColorBase;
   //Subtract of 255 as the value is the transparency value, not the alpha value
   //Cast to uint8 first as the alpha value negative range should be handled unsigned
   const uint8 u8_AlphaValue = 0xFFU - static_cast<uint8>(ocn_Transparency);

   c_Retval.setAlpha(static_cast<sintn>(u8_AlphaValue));
   return c_Retval;
}
