//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for all general properties
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAPROPWIDGET_HPP
#define C_FLAPROPWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaPropWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaPropWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_FlaPropWidget(QWidget * const opc_Parent = NULL);
   ~C_FlaPropWidget(void) override;

   uint8_t GetNodeId(void) const;
   uint32_t GetBitrate(void) const;
   void SetNodeId(const uint8_t ou8_NodeId) const;
   void SetBitrate(const uint32_t ou32_Bitrate) const;

   void SaveUserSettings(void) const;
   void LoadUserSettings(void) const;

private:
   Ui::C_FlaPropWidget * mpc_Ui;

   void m_InitStaticNames(void) const;
   void m_InitStaticGuiElements(void) const;
   //Avoid call
   C_FlaPropWidget(const C_FlaPropWidget &);
   C_FlaPropWidget & operator =(const C_FlaPropWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
