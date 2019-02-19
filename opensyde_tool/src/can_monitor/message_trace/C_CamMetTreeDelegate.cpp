//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Max performance delegate (implementation)

   Max performance delegate

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QBitArray>
#include <QSvgRenderer>
#include <QFontMetrics>
#include <QStyleOptionViewItem>

#include "C_Uti.h"
#include "constants.h"
#include "cam_constants.h"
#include "C_OSCCanSignal.h"
#include "C_TblDelegateUtil.h"
#include "C_CamMetTreeDelegate.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
const QColor C_CamMetTreeDelegate::mhc_HighlightBackgroundColor = mc_STYLE_GUIDE_COLOR_27;
const QColor C_CamMetTreeDelegate::mhc_HighlightForegroundColor = mc_STYLE_GUIDE_COLOR_0;
const QColor C_CamMetTreeDelegate::mhc_HighlightBorderColor = mc_STYLE_GUIDE_COLOR_7;
const QColor C_CamMetTreeDelegate::mhc_DefaultBorderColor = Qt::transparent;
const QColor C_CamMetTreeDelegate::mhc_DefaultBackgroundColor = Qt::transparent;
const QFont C_CamMetTreeDelegate::mhc_HighlightFont = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_12);
const QFont C_CamMetTreeDelegate::mhc_DefaultFont = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_12);

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

   \created     29.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_CamMetTreeDelegate::C_CamMetTreeDelegate(QObject * const opc_Parent) :
   QStyledItemDelegate(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint item

   Here: notify model to unlock data requests (performance)

   \param[in,out] opc_Painter Painter
   \param[in]     orc_Option  Option
   \param[in]     orc_Index   Index

   \created     29.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamMetTreeDelegate::paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                                 const QModelIndex & orc_Index) const
{
   const QRect c_PaddedCellRect = orc_Option.rect.adjusted(3, 1, -3, -2);
   const bool q_Selected = orc_Option.state.testFlag(QStyle::State_Selected);

   Q_EMIT this->SigStartAccept();
   //Evaluate data AFTER unlocking the data
   //Always draw cell (should only be background if any additional paint operations are done)
   QStyledItemDelegate::paint(opc_Painter, orc_Option, orc_Index);
   if (C_TblDelegateUtil::h_PaintMarkedCell(opc_Painter, c_PaddedCellRect, orc_Index, q_Selected,
                                            C_CamMetTreeDelegate::mhc_DefaultBackgroundColor,
                                            C_CamMetTreeDelegate::mhc_DefaultBorderColor,
                                            C_CamMetTreeDelegate::mhc_DefaultFont,
                                            C_CamMetTreeDelegate::mhc_HighlightForegroundColor,
                                            C_CamMetTreeDelegate::mhc_HighlightBackgroundColor,
                                            C_CamMetTreeDelegate::mhc_HighlightBorderColor,
                                            C_CamMetTreeDelegate::mhc_HighlightFont, 19, 25) == true)
   {
      //Don't do anything else
   }
   else if (mh_PaintChildCell(opc_Painter, c_PaddedCellRect, orc_Index, q_Selected) == true)
   {
      //Don't do anything else
   }
   else
   {
      //Nothing to do
   }
   C_CamMetTreeDelegate::mh_PaintSelectedCellIcon(opc_Painter, c_PaddedCellRect, orc_Index,
                                                  q_Selected);
   Q_EMIT this->SigEndAccept();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overloaded size hint

   Adapt the size to get an even number of pixels for each item.
   Reason: Drawing of the points for the branches. Only with an even number of pixel it works out

   \param[in] orc_Option Option
   \param[in] orc_Index  Index

   \return
   Current required size

   \created     28.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QSize C_CamMetTreeDelegate::sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const
{
   QSize c_Size = QStyledItemDelegate::sizeHint(orc_Option, orc_Index);

   c_Size.setHeight(c_Size.height() + 1);

   return c_Size;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint icon

   Warning: no support for other icon positions

   \param[in,out] opc_Painter  Painter
   \param[in]     orc_CellRect Cell rectangle to draw in
   \param[in]     orc_Index    Index
   \param[in]     oq_Selected  Flag if item is selected

   \created     29.01.2019  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamMetTreeDelegate::mh_PaintSelectedCellIcon(QPainter * const opc_Painter, const QRect & orc_CellRect,
                                                    const QModelIndex & orc_Index, const bool oq_Selected)
{
   const QStringList c_IconPaths = orc_Index.data(msn_USER_ROLE_ICON).toStringList();

   //Only handle if icons are set properly
   if (c_IconPaths.size() == 2)
   {
      //Icon seems to have no offset
      const QRect c_IconRect = QRect(orc_CellRect.topLeft(), QSize(16, 16));
      if (oq_Selected)
      {
         QSvgRenderer c_Renderer(c_IconPaths.at(1));
         c_Renderer.render(opc_Painter, c_IconRect);
      }
      else
      {
         QSvgRenderer c_Renderer(c_IconPaths.at(0));
         c_Renderer.render(opc_Painter, c_IconRect);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paint child section

   \param[in,out] opc_Painter  Painter
   \param[in]     orc_CellRect Cell rectangle to draw in
   \param[in]     orc_Index    Index
   \param[in]     oq_Selected  Flag if item is selected

   \return
   True  Section painted
   False Section empty

   \created     28.09.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_CamMetTreeDelegate::mh_PaintChildCell(QPainter * const opc_Painter, const QRect & orc_CellRect,
                                             const QModelIndex & orc_Index, const bool oq_Selected)
{
   bool q_Retval = true;

   if (orc_Index.parent().isValid() == true)
   {
      //Handle manually
      const QStringList c_StringParts = orc_Index.data(msn_USER_ROLE_STRING_PARTS).toStringList();
      const std::vector<sint32> c_ColSizes = mh_GetChildColWidths();
      const std::vector<QString> c_Spaces = C_CamMetTreeDelegate::mh_GetTopSpaces();
      const std::vector<QFlags<Qt::AlignmentFlag> > c_Alignments = C_CamMetTreeDelegate::mh_GetTopAlignmentFlags();
      QRect c_CellRectAdapted;

      // Space for icon necessary
      c_CellRectAdapted = orc_CellRect.adjusted(20, 0, 0, 0);

      opc_Painter->save();
      if (((c_ColSizes.size() == static_cast<uint32>(c_StringParts.size())) &&
           (c_ColSizes.size() == c_Alignments.size())) && (c_ColSizes.size() == (c_Spaces.size() + 1UL)))
      {
         uint32 u32_It = 0;
         QPoint c_TopLeft = c_CellRectAdapted.topLeft();
         QColor c_FontColor;

         if (oq_Selected == true)
         {
            c_FontColor = C_CamMetTreeDelegate::mhc_HighlightForegroundColor;
         }
         else
         {
            //Look for color in PARENT!
            c_FontColor = orc_Index.parent().data(static_cast<sint32>(Qt::ForegroundRole)).value<QColor>();
         }

         for (QStringList::const_iterator c_It = c_StringParts.begin(); c_It != c_StringParts.end(); ++c_It)
         {
            bool q_Break = false;

            //Handle cell content
            if (C_TblDelegateUtil::h_PaintStringWithRestriction(opc_Painter, c_TopLeft, c_CellRectAdapted, *c_It,
                                                                C_CamMetTreeDelegate::mhc_DefaultBackgroundColor,
                                                                C_CamMetTreeDelegate::mhc_DefaultBorderColor,
                                                                c_FontColor,
                                                                C_CamMetTreeDelegate::mhc_DefaultFont,
                                                                c_Alignments[u32_It],
                                                                c_ColSizes[u32_It]) == true)
            {
               q_Break = true;
            }
            else if (u32_It < c_Spaces.size())
            {
               //Handle space in between
               if (C_TblDelegateUtil::h_PaintStringWithRestriction(opc_Painter, c_TopLeft, c_CellRectAdapted,
                                                                   c_Spaces[u32_It],
                                                                   C_CamMetTreeDelegate::mhc_DefaultBackgroundColor,
                                                                   C_CamMetTreeDelegate::mhc_DefaultBorderColor,
                                                                   c_FontColor,
                                                                   C_CamMetTreeDelegate::mhc_DefaultFont,
                                                                   Qt::AlignLeft | Qt::AlignVCenter,
                                                                   -1) == true)
               {
                  q_Break = true;
               }
            }
            else
            {
               // Nothing to do
            }

            if (q_Break == true)
            {
               break;
            }

            //Important iterator step
            ++u32_It;
         }
      }
      opc_Painter->restore();
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get maximum length of strings

   \param[in] orc_Names Strings to evaluate

   \return
   Maximum length of strings

   \created     26.09.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamMetTreeDelegate::mh_GetMaxLength(const QStringList & orc_Names)
{
   sint32 s32_Retval = 0;

   for (QStringList::const_iterator c_ItName = orc_Names.begin(); c_ItName != orc_Names.end(); ++c_ItName)
   {
      const QFontMetrics c_MetricsDefault(C_CamMetTreeDelegate::mhc_DefaultFont);
      const QString & rc_Name = *c_ItName;
      const sint32 s32_Length = c_MetricsDefault.width(rc_Name);
      s32_Retval = std::max(s32_Retval, s32_Length);
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get vector of all restricted column sizes

   \return
   Vector of all restricted column sizes

   \created     28.09.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<sint32> C_CamMetTreeDelegate::mh_GetChildColWidths(void)
{
   std::vector<sint32> c_Retval;
   c_Retval.reserve(5);
   //Signal name
   c_Retval.push_back(200);
   //Signal value (phys)
   c_Retval.push_back(150);
   //Signal unit
   c_Retval.push_back(50);
   //Signal value (raw)
   c_Retval.push_back(50);
   //Special value for last column
   c_Retval.push_back(-1);
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get alignment flags for top items

   \return
   Alignment flags for top items

   \created     01.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<QFlags<Qt::AlignmentFlag> > C_CamMetTreeDelegate::mh_GetTopAlignmentFlags(void)
{
   std::vector<QFlags<Qt::AlignmentFlag> > c_Retval;
   c_Retval.push_back(Qt::AlignLeft | Qt::AlignVCenter);
   c_Retval.push_back(Qt::AlignRight | Qt::AlignVCenter);
   c_Retval.push_back(Qt::AlignLeft | Qt::AlignVCenter);
   c_Retval.push_back(Qt::AlignRight | Qt::AlignVCenter);
   c_Retval.push_back(Qt::AlignLeft | Qt::AlignVCenter);
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get spaces between top items

   \return
   Spaces between top items

   \created     01.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<QString> C_CamMetTreeDelegate::mh_GetTopSpaces(void)
{
   std::vector<QString> c_Retval;
   c_Retval.push_back("");
   c_Retval.push_back(" ");
   c_Retval.push_back("");
   c_Retval.push_back(" (raw)              ");
   return c_Retval;
}
