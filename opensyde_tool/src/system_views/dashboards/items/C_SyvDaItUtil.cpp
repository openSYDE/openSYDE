//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for dashboard widget items (implementation)

   Utility class for dashboard widget items

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvDaItUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy font size of one label to another

   \param[in]     opc_SourceLabel Source label
   \param[in,out] opc_TargetLabel Target label
   \param[in]     of32_Factor     Optional factor to apply to point size on copy action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItUtil::h_CopyFontSize(const QLabel * const opc_SourceLabel, QLabel * const opc_TargetLabel,
                                   const float32_t of32_Factor)
{
   if ((opc_SourceLabel != NULL) && (opc_TargetLabel != NULL))
   {
      QFont c_Font;
      const float32_t f32_SourcePointSize = static_cast<float32_t>(opc_SourceLabel->font().pointSize());
      const float32_t f32_SourcePointSizeWithFactor = f32_SourcePointSize * of32_Factor;
      const int32_t s32_PointSize = std::max(static_cast<int32_t>(f32_SourcePointSizeWithFactor), 1);

      c_Font = opc_TargetLabel->font();
      c_Font.setPointSize(s32_PointSize);
      opc_TargetLabel->setFont(c_Font);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync font for up to four labels (choose minimum supported value)

   \param[in,out] opc_Label1 Label 1 to sync font for
   \param[in,out] opc_Label2 Label 2 to sync font for
   \param[in,out] opc_Label3 Label 3 to sync font for
   \param[in,out] opc_Label4 Label 4 to sync font for
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItUtil::h_SyncFontSize(QLabel * const opc_Label1, QLabel * const opc_Label2, QLabel * const opc_Label3,
                                   QLabel * const opc_Label4)
{
   if ((opc_Label1 != NULL) && (opc_Label2 != NULL))
   {
      QFont c_Font;
      int32_t s32_PointSize;
      s32_PointSize = std::min(opc_Label1->font().pointSize(), opc_Label2->font().pointSize());
      if (opc_Label3 != NULL)
      {
         s32_PointSize = std::min(s32_PointSize, opc_Label3->font().pointSize());
      }
      if (opc_Label4 != NULL)
      {
         s32_PointSize = std::min(s32_PointSize, opc_Label4->font().pointSize());
      }

      s32_PointSize = std::max(s32_PointSize, 1);

      //1
      c_Font = opc_Label1->font();
      c_Font.setPointSize(s32_PointSize);
      opc_Label1->setFont(c_Font);

      //2
      c_Font = opc_Label2->font();
      c_Font.setPointSize(s32_PointSize);
      opc_Label2->setFont(c_Font);

      //3
      if (opc_Label3 != NULL)
      {
         c_Font = opc_Label3->font();
         c_Font.setPointSize(s32_PointSize);
         opc_Label3->setFont(c_Font);
      }

      //4
      if (opc_Label4 != NULL)
      {
         c_Font = opc_Label4->font();
         c_Font.setPointSize(s32_PointSize);
         opc_Label4->setFont(c_Font);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Indent some text

   \param[in] ou32_NumberOfTimes Number of indentations

   \return
   Style with indentations
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItUtil::h_GetHtmlIndentStyle(const uint32_t ou32_NumberOfTimes)
{
   return static_cast<QString>("style=\"margin-left:%1px\"").arg(ou32_NumberOfTimes * 10);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItUtil::C_SyvDaItUtil(void)
{
}
