//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing information and offering configuration of one chart data series (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITCHARTDATAITEMWIDGET_H
#define C_SYVDAITCHARTDATAITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPixmap>

#include "stwtypes.h"
#include "C_OgeWiWithToolTip.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItChartDataItemWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItChartDataItemWidget :
   public stw_opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SyvDaItChartDataItemWidget(const stw_types::uint32 ou32_ViewIndex, QWidget * const opc_Parent = NULL);
   virtual ~C_SyvDaItChartDataItemWidget();

   void InitWidget(const stw_types::uint32 ou32_DataPoolElementConfigIndex,
                   const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                   const QString & orc_DisplayName, const bool oq_Active, const QColor & orc_DataPoolElementColor,
                   const QString & orc_Unit, const bool oq_Warning = false, const bool oq_Invalid = false,
                   const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType = stw_opensyde_core::C_OSCNodeDataPool::eDIAG, const QString & orc_ToolTipErrorTextHeading = "", const QString & orc_ToolTipErrorText = "");
   void UpdateIndex(const stw_types::uint32 ou32_DataPoolElementConfigIndex);

   void UpdateValue(const QString & orc_Value) const;
   void UpdateError(const QString & orc_ErrorText, const bool oq_IsTransmissionError, const bool oq_ErrorActive);
   void ResetError(void);
   void UpdateTransparence(const stw_types::sintn osn_Value) const;

   QString GetDataElementName(void) const;
   QString GetDataElementUnit(void) const;
   void SetDataElementUnit(const QString & orc_DisplayName, const QString & orc_Unit);

   void SetDataElementSelected(const bool oq_Selected);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDataItemToggled(const stw_types::uint32 ou32_DataPoolElementConfigIndex, const bool oq_Checked);
   void SigDataItemSelected(const stw_types::uint32 ou32_DataPoolElementConfigIndex);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_SyvDaItChartDataItemWidget * mpc_Ui;
   const stw_types::uint32 mu32_ViewIndex;
   stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId mc_DataPoolElementId;
   stw_types::uint32 mu32_DataPoolElementConfigIndex;
   bool mq_Selected;
   bool mq_Warning;
   bool mq_Error;
   bool mq_Invalid;

   QString mc_ToolTipErrorTextHeading;
   QString mc_ToolTipErrorText;
   stw_opensyde_core::C_OSCNodeDataPool::E_Type me_PlaceholderDataPoolType;

   QString mc_Unit;

   QPixmap mc_IconSignal;
   QPixmap mc_IconVariable;
   QPixmap mc_IconParameter;
   QPixmap mc_IconSignalWarning;
   QPixmap mc_IconVariableWarning;
   QPixmap mc_IconParameterWarning;
   QPixmap mc_IconSignalError;
   QPixmap mc_IconVariableError;
   QPixmap mc_IconParameterError;

   void m_DataItemToggled(const bool oq_Checked);
   void m_UpdateIcon(void) const;

   //Avoid call
   C_SyvDaItChartDataItemWidget(const C_SyvDaItChartDataItemWidget &);
   C_SyvDaItChartDataItemWidget & operator =(const C_SyvDaItChartDataItemWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
