//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for showing message layout

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEMLVWIDGET_H
#define C_SDBUEMLVWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QCursor>

#include "C_SdBueMlvGraphicsScene.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"
#include "C_OSCCanProtocol.h"
/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SdBueMlvWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueMlvWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMlvWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueMlvWidget(void);
   void SetMessageSyncManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value);
   void SetComProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value);
   void SelectMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void SelectSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint32 ou32_SignalIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMessageUpdated(void);
   void SigSignalActivated(const stw_types::uint32 ou32_SignalIndex);

   void SigAddSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint16 ou16_StartBit);
   void SigCopySignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                      const stw_types::uint32 ou32_SignalIndex);
   void SigCutSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                     const stw_types::uint32 ou32_SignalIndex);
   void SigPasteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                       const stw_types::uint16 ou16_StartBit);
   void SigDeleteSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                        const stw_types::uint32 ou32_SignalIndex);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdBueMlvWidget(const C_SdBueMlvWidget &);
   C_SdBueMlvWidget & operator =(const C_SdBueMlvWidget &);

   void m_ChangeCursor(const Qt::CursorShape oe_Cursor);
   void m_OnMessageUpdated(void);
   void m_OnSignalActivated(const stw_types::uint32 ou32_SignalIndex);

   Ui::C_SdBueMlvWidget * mpc_Ui;
   C_SdBueMlvGraphicsScene * mpc_Scene;
};
}

#endif
