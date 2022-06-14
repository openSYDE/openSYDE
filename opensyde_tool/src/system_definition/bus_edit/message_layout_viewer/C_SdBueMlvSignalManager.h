//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Managing the graphics items for a signal in the message layout viewer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMLVSIGNALMANAGER_H
#define C_SDBUEMLVSIGNALMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include <QVector>
#include <QColor>
#include <set>

#include "stwtypes.h"

#include "C_GiBiCustomToolTip.h"
#include "C_SdBueMlvSignalItem.h"
#include "C_OSCCanSignal.h"
#include "C_PuiSdNodeCanSignal.h"
#include "C_OSCCanProtocol.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_PuiSdNodeCanMessageSyncManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvSignalManager :
   public QObject,
   public C_GiBiCustomToolTip
{
   Q_OBJECT

public:
   // internal class for color configuration
   class C_SignalItemColors
   {
   public:
      stw_types::uint8 u8_Index;
      QColor c_BackgroundColor;
      QColor c_FontColor;
      QColor c_ResizeItemColor;
   };

   enum E_InterActionMode
   {
      eIAM_NONE,
      eIAM_RESIZELEFT_INTEL,
      eIAM_RESIZERIGHT_INTEL,
      eIAM_RESIZELEFT_MOTOROLA,
      eIAM_RESIZERIGHT_MOTOROLA,
      eIAM_MOVE
   };

   C_SdBueMlvSignalManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_SyncManager,
                           const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                           const stw_types::uint16 ou16_MaximumCountBits, const stw_types::float64 of64_Space,
                           QObject * const opc_Parent = NULL);
   virtual ~C_SdBueMlvSignalManager();

   void LoadSignal(const stw_types::uint32 ou32_SignalIndex, const C_SignalItemColors & orc_ColorConfiguration,
                   const bool oq_Resizeable);
   void SetColorConfiguration(const C_SignalItemColors & orc_ColorConfiguration);
   void SetName(const QString & orc_Name);
   bool SetStartBit(const stw_types::uint16 ou16_Position);
   bool SetLastBit(const stw_types::uint16 ou16_Position);
   void SetZOrder(const stw_types::float64 of64_ZOrder) const;
   stw_types::float64 GetZOrder(void) const;
   void SetSelected(const bool oq_Selected);

   bool MoveSignal(const stw_types::sint32 os32_Offset);

   QString GetName(void) const;
   stw_types::uint16 GetStartBit(void) const;
   stw_types::uint16 GetLastBit(void) const;
   stw_types::uint16 GetLength(void) const;
   stw_types::uint32 GetSignalIndex(void) const;
   C_SignalItemColors GetColorConfiguration(void) const;
   E_InterActionMode GetInteractionMode(const QPointF & orc_MousePos) const;

   bool IsHovered(void) const;
   virtual void GenerateHint(void) override;

   void Update(const stw_types::float64 of64_SingleItemWidth, const stw_types::float64 of64_SingleItemHeight);
   void ClearItems(void);
   void SaveSignal(void) const;

   // Helper functions for converting byte order
   stw_types::uint16 GetDataBytesBitPosOfSignalBit(const stw_types::uint16 ou16_SignalBitPosition) const;
   stw_types::uint16 GetDataBytesBitPosOfSignalBit(const stw_types::uint16 ou16_StartBit,
                                                   const stw_types::uint16 ou16_SignalBitPosition) const;
   void GetDataBytesBitPositionsOfSignal(std::set<stw_types::uint16> & orc_SetPositions) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigAddItem(C_SdBueMlvSignalItem * const opc_Item);
   void SigRemoveItem(C_SdBueMlvSignalItem * const opc_Item);
   void SigChangeCursor(const Qt::CursorShape oe_Cursor);
   void SigHideToolTip(void);

private:
   //Avoid call
   C_SdBueMlvSignalManager(const C_SdBueMlvSignalManager &);
   C_SdBueMlvSignalManager & operator =(const C_SdBueMlvSignalManager &);

   void m_UpdateItemConfiguration(void);
   void m_SignalItemHovered(const bool oq_Hover);
   void m_SetNewLength(const stw_types::sint16 os16_Lenth);
   void m_SetError(const bool oq_Active);
   bool m_IsLengthValid(const stw_types::sint16 os16_Length) const;

   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const mpc_MessageSyncManager;

   // identification informations about the message
   const stw_opensyde_core::C_OSCCanMessageIdentificationIndices mc_MessageId;
   stw_types::uint16 mu16_MaximumCountBits;
   stw_types::uint32 mu32_SignalIndex;

   // the shown parts of the signal
   QVector<C_SdBueMlvSignalItem *> mc_VecSignalItems;

   QString mc_Name;
   bool mq_MultiplexerSignal;
   C_SignalItemColors mc_ColorConfiguration;
   stw_types::uint16 mu16_LastBit;
   stw_opensyde_core::C_OSCCanSignal mc_Signal;
   stw_opensyde_gui_logic::C_PuiSdNodeCanSignal mc_UiSignal;
   bool mq_Resizeable;
   bool mq_Hovered;
   const stw_types::float64 mf64_Space;
   const stw_types::sint16 ms16_MaximumLength;
   const stw_types::sint16 ms16_MaximumLengthMultiplexer;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
