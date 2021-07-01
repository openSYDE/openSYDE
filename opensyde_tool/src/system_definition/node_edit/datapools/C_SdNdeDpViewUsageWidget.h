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

#include "stwtypes.h"

#include "C_NagToolTip.h"
#include "C_PuiSdHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpViewUsageWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpViewUsageWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpViewUsageWidget(void);

   void SetUsage(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_MaximumSize,
                 const std::vector<stw_opensyde_gui_logic::C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas, stw_types::uint32 & oru32_Percentage, stw_types::uint32 & oru32_UsedNvmSize);
   void DataPoolHoverStateChanged(const stw_types::uint32 ou32_DataPoolIndex, const bool oq_Hovered);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigHideOtherToolTips(void);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;

private:
   class C_SdNdeDpViewUsageArea
   {
   public:
      QString c_ShowedString;
      QColor c_Color;
      stw_types::uint32 u32_Percentage;
      stw_types::float32 f32_PercentageFloat;
      stw_types::float32 f32_PercentageFloatDatapool; // In case of a Datapool over multiple areas different to
                                                      // f32_PercentageFloat
      stw_types::uint32 u32_StartAddressDatapool;     // In case of a Datapool over multiple areas different to
                                                      // c_NvmArea.u32_StartAddress
      stw_types::uint32 u32_SizeDatapool;             // In case of a Datapool over multiple areas different to
                                                      // c_NvmArea.u32_Size
      stw_opensyde_gui_logic::C_PuiSdHandlerNodeLogic::C_PuiSdHandlerNodeLogicNvmArea c_NvmArea;
      bool q_Used;
   };

   QBrush mc_Brush;
   QPen mc_Pen;
   QColor mc_ColorFree;
   C_NagToolTip * mpc_ToolTip;
   stw_types::uint32 mu32_TotalUsed;
   stw_types::uint32 mu32_TotalPercentage;
   stw_types::uint32 mu32_Size;
   std::vector<C_SdNdeDpViewUsageArea> mc_VecUsageAreas;
   std::vector<stw_types::sintn> mc_Distances;

   bool mq_DataPoolHovered;
   stw_types::uint32 mu32_HoveredDataPoolIndex;

   stw_types::uint32 m_GetUsageAreaIndex(const stw_types::sintn osn_PosX);
   bool m_CheckForContainingDataPool(const stw_types::uint32 ou32_AreaIndex,
                                     const stw_types::uint32 ou32_DataPoolIndex);
   bool m_CheckForContainingDataPool(const stw_types::uint32 ou32_AreaIndex,
                                     const std::vector<stw_types::uint32> & orc_DataPoolIndexes);
   void m_UpdateTooltip(const stw_types::sintn osn_MouseX);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
