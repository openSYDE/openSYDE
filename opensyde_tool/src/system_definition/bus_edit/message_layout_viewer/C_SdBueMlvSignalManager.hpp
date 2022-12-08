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

#include "stwtypes.hpp"

#include "C_GiBiCustomToolTip.hpp"
#include "C_SdBueMlvSignalItem.hpp"
#include "C_OscCanSignal.hpp"
#include "C_PuiSdNodeCanSignal.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_PuiSdNodeCanMessageSyncManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
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
      uint8_t u8_Index;
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

   C_SdBueMlvSignalManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_SyncManager,
                           const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                           const uint16_t ou16_MaximumCountBits, const float64_t of64_Space,
                           QObject * const opc_Parent = NULL);
   virtual ~C_SdBueMlvSignalManager();

   void LoadSignal(const uint32_t ou32_SignalIndex, const C_SignalItemColors & orc_ColorConfiguration,
                   const bool oq_Resizeable);
   void SetColorConfiguration(const C_SignalItemColors & orc_ColorConfiguration);
   void SetName(const QString & orc_Name);
   bool SetStartBit(const uint16_t ou16_Position);
   bool SetLastBit(const uint16_t ou16_Position);
   void SetZeOrder(const float64_t of64_ZetOrder) const;
   float64_t GetZeOrder(void) const;
   void SetSelected(const bool oq_Selected);

   bool MoveSignal(const int32_t os32_Offset);

   QString GetName(void) const;
   uint16_t GetStartBit(void) const;
   uint16_t GetLastBit(void) const;
   uint16_t GetLength(void) const;
   uint32_t GetSignalIndex(void) const;
   C_SignalItemColors GetColorConfiguration(void) const;
   E_InterActionMode GetInteractionMode(const QPointF & orc_MousePos) const;

   bool IsHovered(void) const;
   void GenerateHint(void) override;

   void Update(const float64_t of64_SingleItemWidth, const float64_t of64_SingleItemHeight);
   void ClearItems(void);
   void SaveSignal(void) const;

   // Helper functions for converting byte order
   uint16_t GetDataBytesBitPosOfSignalBit(const uint16_t ou16_SignalBitPosition) const;
   uint16_t GetDataBytesBitPosOfSignalBit(const uint16_t ou16_StartBit, const uint16_t ou16_SignalBitPosition) const;
   void GetDataBytesBitPositionsOfSignal(std::set<uint16_t> & orc_SetPositions) const;

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
   C_SdBueMlvSignalManager & operator =(const C_SdBueMlvSignalManager &) &;

   void m_UpdateItemConfiguration(void);
   void m_SignalItemHovered(const bool oq_Hover);
   void m_SetNewLength(const int16_t os16_Lenth);
   void m_SetError(const bool oq_Active);
   bool m_IsLengthValid(const int16_t os16_Length) const;

   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const mpc_MessageSyncManager;

   // identification information about the message
   const stw::opensyde_core::C_OscCanMessageIdentificationIndices mc_MessageId;
   uint16_t mu16_MaximumCountBits;
   uint32_t mu32_SignalIndex;

   // the shown parts of the signal
   QVector<C_SdBueMlvSignalItem *> mc_VecSignalItems;

   QString mc_Name;
   bool mq_MultiplexerSignal;
   C_SignalItemColors mc_ColorConfiguration;
   uint16_t mu16_LastBit;
   stw::opensyde_core::C_OscCanSignal mc_Signal;
   stw::opensyde_gui_logic::C_PuiSdNodeCanSignal mc_UiSignal;
   bool mq_Resizeable;
   bool mq_Hovered;
   const float64_t mf64_Space;
   const int16_t ms16_MaximumLength;
   const int16_t ms16_MaximumLengthMultiplexer;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
