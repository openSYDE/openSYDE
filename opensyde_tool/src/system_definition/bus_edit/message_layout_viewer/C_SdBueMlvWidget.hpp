//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing message layout

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMLVWIDGET_H
#define C_SDBUEMLVWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <map>
#include <QWidget>
#include <QCursor>

#include "C_SdBueMlvGraphicsScene.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"
#include "C_OscCanProtocol.hpp"
/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueMlvWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMlvWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueMlvWidget(void) override;

   void InitStaticNames(void) const;

   void SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void SetComProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Value);
   void SelectMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void SelectSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t ou32_SignalIndex);
   void RefreshColors(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMessageUpdated(void);
   void SigSignalActivated(const uint32_t ou32_SignalIndex);

   void SigAddSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint16_t ou16_StartBit);
   void SigAddSignalMultiplexed(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                const uint16_t ou16_StartBit, const uint16_t ou16_MultiplexValue);
   void SigCopySignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                      const uint32_t ou32_SignalIndex);
   void SigCutSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                     const uint32_t ou32_SignalIndex);
   void SigPasteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                       const uint16_t ou16_StartBit);
   void SigDeleteSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                        const uint32_t ou32_SignalIndex);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueMlvWidget(const C_SdBueMlvWidget &);
   C_SdBueMlvWidget & operator =(const C_SdBueMlvWidget &) &;

   void m_SelectMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_ChangeCursor(const Qt::CursorShape oe_Cursor);
   void m_OnMessageUpdated(void);
   void m_OnMultiplexerValueChanged(void);
   void m_OnSignalActivated(const uint32_t ou32_SignalIndex);
   void m_UpdateMultiplexerValues(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                  const bool oq_SetSpecificMultiplexerValue, const uint16_t ou16_MultiplexerValue);
   uint16_t m_GetSelectedMultiplexerValues(void) const;
   QString m_GetComboBoxEntryByMultiplexerValue(const uint16_t ou16_Value) const;

   Ui::C_SdBueMlvWidget * mpc_Ui;
   C_SdBueMlvGraphicsScene * mpc_Scene;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_Protocol;
   stw::opensyde_core::C_OscCanMessageIdentificationIndices mc_PreviousMessageId;
   // Mapping from combo box string to concrete value
   std::map<QString, uint16_t> mc_MultiplexerValues;
};
}
}

#endif
