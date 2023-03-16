//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom font configuration widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIFONTCONFIG_H
#define C_OGEWIFONTCONFIG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_OgeWiFontConfig;
}

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OgeWiFontConfig :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiFontConfig(QWidget * const opc_Parent = NULL);
   ~C_OgeWiFontConfig(void) override;

   void InitStaticNames(void) const;
   void SetFont(const QFont & orc_Font);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigFontUpdate(const QFont & orc_Font);

private:
   Ui::C_OgeWiFontConfig * mpc_Ui;
   static const int32_t mhs32_INDEX_SIZE_8;
   static const int32_t mhs32_INDEX_SIZE_9;
   static const int32_t mhs32_INDEX_SIZE_10;
   static const int32_t mhs32_INDEX_SIZE_11;
   static const int32_t mhs32_INDEX_SIZE_12;
   static const int32_t mhs32_INDEX_SIZE_14;
   static const int32_t mhs32_INDEX_SIZE_16;
   static const int32_t mhs32_INDEX_SIZE_18;
   static const int32_t mhs32_INDEX_SIZE_20;
   static const int32_t mhs32_INDEX_SIZE_22;
   static const int32_t mhs32_INDEX_SIZE_24;
   static const int32_t mhs32_INDEX_SIZE_26;
   static const int32_t mhs32_INDEX_SIZE_28;
   static const int32_t mhs32_INDEX_SIZE_36;
   static const int32_t mhs32_INDEX_SIZE_48;
   static const int32_t mhs32_INDEX_SIZE_72;
   static const int32_t mhs32_INDEX_SIZE_100;
   static const int32_t mhs32_INDEX_SIZE_200;
   static const int32_t mhs32_INDEX_SIZE_300;
   static const int32_t mhs32_INDEX_SIZE_400;
   static const int32_t mhs32_INDEX_SIZE_500;

   void m_TriggerUpdate(void);
   int32_t m_GetSizeIndex(const QFont & orc_Font) const;
   void m_GetSize(QFont & orc_Font) const;

   //Avoid call
   C_OgeWiFontConfig(const C_OgeWiFontConfig &);
   C_OgeWiFontConfig & operator =(const C_OgeWiFontConfig &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace
#endif
