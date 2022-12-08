//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar with multiple elements (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPVIEWUSAGEWIDGET_H
#define C_SDNDEDPVIEWUSAGEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <vector>

#include "stwtypes.hpp"

#include "C_NagToolTip.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpViewUsageWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpViewUsageWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpViewUsageWidget(void) override;

   void SetUsage(const uint32_t ou32_NodeIndex, const uint32_t ou32_MaximumSize,
                 const std::vector<stw::opensyde_gui_logic::C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas, uint32_t & oru32_Percentage, uint32_t & oru32_UsedNvmSize);
   void DataPoolHoverStateChanged(const uint32_t ou32_DataPoolIndex, const bool oq_Hovered);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigHideOtherToolTips(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;

private:
   class C_SdNdeDpViewUsageArea
   {
   public:
      QString c_ShowedString;
      QColor c_Color;
      uint32_t u32_Percentage;
      float32_t f32_PercentageFloat;
      float32_t f32_PercentageFloatDatapool; // In case of a Datapool over multiple areas different to
      // f32_PercentageFloat
      uint32_t u32_StartAddressDatapool; // In case of a Datapool over multiple areas different to
      // c_NvmArea.u32_StartAddress
      uint32_t u32_SizeDatapool; // In case of a Datapool over multiple areas different to
      // c_NvmArea.u32_Size
      stw::opensyde_gui_logic::C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea c_NvmArea;
      bool q_Used;
   };

   QBrush mc_Brush;
   QPen mc_Pen;
   QColor mc_ColorFree;
   C_NagToolTip * mpc_ToolTip;
   uint32_t mu32_TotalUsed;
   uint32_t mu32_TotalPercentage;
   uint32_t mu32_Size;
   std::vector<C_SdNdeDpViewUsageArea> mc_VecUsageAreas;
   std::vector<int32_t> mc_Distances;

   bool mq_DataPoolHovered;
   uint32_t mu32_HoveredDataPoolIndex;

   uint32_t m_GetUsageAreaIndex(const int32_t os32_PosHorizontal);
   bool m_CheckForContainingDataPool(const uint32_t ou32_AreaIndex, const uint32_t ou32_DataPoolIndex);
   bool m_CheckForContainingDataPool(const uint32_t ou32_AreaIndex, const std::vector<uint32_t> & orc_DataPoolIndexes);
   void m_UpdateTooltip(const int32_t os32_MouseHorizontal);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
