//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Utility class for dashboard widget items (implementation)

   Utility class for dashboard widget items

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SyvDaItUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Init item font family

   \param[in,out] opc_Label Label to init font family

   \created     22.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItUtil::h_InitFontFamily(QLabel * const opc_Label)
{
   if (opc_Label != NULL)
   {
      QFont c_Font;

      c_Font = opc_Label->font();
      c_Font.setFamily("Segoe UI");
      opc_Label->setFont(c_Font);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy font size of one label to another

   \param[in]     opc_SourceLabel Source label
   \param[in,out] opc_TargetLabel Target label
   \param[in]     of32_Factor     Optional factor to apply to point size on copy action

   \created     22.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItUtil::h_CopyFontSize(const QLabel * const opc_SourceLabel, QLabel * const opc_TargetLabel,
                                   const float32 of32_Factor)
{
   if ((opc_SourceLabel != NULL) && (opc_TargetLabel != NULL))
   {
      QFont c_Font;
      const float32 f32_SourcePointSize = static_cast<float32>(opc_SourceLabel->font().pointSize());
      const float32 f32_SourcePointSizeWithFactor = f32_SourcePointSize * of32_Factor;
      const sintn sn_PointSize = std::max(static_cast<sintn>(f32_SourcePointSizeWithFactor), 1);

      c_Font = opc_TargetLabel->font();
      c_Font.setPointSize(sn_PointSize);
      opc_TargetLabel->setFont(c_Font);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sync font for up to four labels (choose minimum supported value)

   \param[in,out] opc_Label1 Label 1 to sync font for
   \param[in,out] opc_Label2 Label 2 to sync font for
   \param[in,out] opc_Label3 Label 3 to sync font for
   \param[in,out] opc_Label4 Label 4 to sync font for

   \created     22.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItUtil::h_SyncFontSize(QLabel * const opc_Label1, QLabel * const opc_Label2, QLabel * const opc_Label3,
                                   QLabel * const opc_Label4)
{
   if ((opc_Label1 != NULL) && (opc_Label2 != NULL))
   {
      QFont c_Font;
      sintn sn_PointSize;
      sn_PointSize = std::min(opc_Label1->font().pointSize(), opc_Label2->font().pointSize());
      if (opc_Label3 != NULL)
      {
         sn_PointSize = std::min(sn_PointSize, opc_Label3->font().pointSize());
      }
      if (opc_Label4 != NULL)
      {
         sn_PointSize = std::min(sn_PointSize, opc_Label4->font().pointSize());
      }

      sn_PointSize = std::max(sn_PointSize, 1);

      //1
      c_Font = opc_Label1->font();
      c_Font.setPointSize(sn_PointSize);
      opc_Label1->setFont(c_Font);

      //2
      c_Font = opc_Label2->font();
      c_Font.setPointSize(sn_PointSize);
      opc_Label2->setFont(c_Font);

      //3
      if (opc_Label3 != NULL)
      {
         c_Font = opc_Label3->font();
         c_Font.setPointSize(sn_PointSize);
         opc_Label3->setFont(c_Font);
      }

      //4
      if (opc_Label4 != NULL)
      {
         c_Font = opc_Label4->font();
         c_Font.setPointSize(sn_PointSize);
         opc_Label4->setFont(c_Font);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Indent some text

   \param[in] ou32_NumberOfTimes Number of indentations

   \return
   Style with indentations

   \created     04.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SyvDaItUtil::mh_GetHtmlIndentStyle(const uint32 ou32_NumberOfTimes)
{
   return QString("style=\"margin-left:%1px\"").arg(ou32_NumberOfTimes * 10);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     11.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItUtil::C_SyvDaItUtil(void)
{
}
